# Kiến trúc phần mềm

Tài liệu này mô tả chi tiết thiết kế phần mềm của game Tetris trên STM32F429I Discovery. Xem [README](../README.md) để có cái nhìn tổng quan, và [HARDWARE.md](HARDWARE.md) cho phần nối dây.

---

## 1. Nguyên tắc thiết kế

Yêu cầu đề bài là chơi được bằng **cả** nút bấm lẫn cử chỉ tay. Cách làm ngây thơ là để logic game hỏi trực tiếp "đang ở chế độ nào?" rồi rẽ nhánh đọc GPIO hoặc đọc I²C. Cách đó khiến logic game dính chặt vào phần cứng, và mỗi lần thêm nguồn điều khiển mới lại phải sửa game.

Thiết kế của dự án đảo ngược quan hệ đó:

> **Nguồn điều khiển đẩy lệnh vào queue. Game chỉ đọc queue.**

Logic game không hề biết — và không cần biết — lệnh đến từ đâu. Hệ quả:

- Đổi chế độ lúc đang chạy không cần reset game hay khởi tạo lại màn hình.
- Thêm nguồn điều khiển thứ ba (ví dụ UART, cảm biến nghiêng) chỉ cần thêm một task đẩy vào cùng Queue1.
- Test logic game không cần phần cứng: chỉ cần bơm byte vào Queue1.

---

## 2. Sơ đồ luồng dữ liệu

```
   ┌────────────────────┐                    ┌────────────────────┐
   │      myTask03      │                    │      myTask04      │
   │  quét 4 nút bấm    │                    │  đọc PAJ7620 I²C3  │
   │  chu kỳ 10 ms      │                    │  chu kỳ 10 ms      │
   │  (mặc định: CHẠY)  │                    │ (mặc định: SUSPEND)│
   └─────────┬──────────┘                    └─────────┬──────────┘
             │                                         │
             │   osMessageQueuePut('L'|'R'|'D'|'C')    │
             └──────────────────┬──────────────────────┘
                                ▼
                    ╔═══════════════════════╗
                    ║        Queue1         ║  8 × uint8_t
                    ║   lệnh điều khiển     ║
                    ╚═══════════╤═══════════╝
                                │ osMessageQueueGet
                                ▼
                    ┌───────────────────────┐
                    │       GUI_Task        │
                    │  TouchGFX render loop │
                    │  + logic Tetris       │
                    └───────────┬───────────┘
                                │ osMessageQueuePut(0 | 1)
                                ▼
                    ╔═══════════════════════╗
                    ║        Queue2         ║  8 × uint8_t
                    ║   chế độ điều khiển   ║
                    ╚═══════════╤═══════════╝
                                │ osMessageQueueGet
                                ▼
                    ┌───────────────────────┐
                    │      defaultTask      │
                    │  updateTasksBasedOn   │
                    │        Mode()         │
                    └───────────┬───────────┘
                                │
                 ┌──────────────┴──────────────┐
                 ▼                             ▼
        osThreadResume/Suspend         HAL_GPIO_WritePin
          myTask03 ↔ myTask04            PG13 / PG14 (LED)
```

---

## 3. Danh sách task

Tất cả task được tạo trong `MX_FREERTOS_Init()` tại [`Core/Src/main.c`](../Core/Src/main.c).

| Task | Hàm vào | Trạng thái khởi tạo | Vai trò |
|---|---|---|---|
| `GUI_Task` | `TouchGFX_Task` | Chạy | Render TouchGFX, chạy logic Tetris, tiêu thụ Queue1, sinh Queue2 |
| `myTask03` | `StartTask03` | Chạy | Quét nút bấm → Queue1 |
| `myTask04` | `StartTask04` | **Suspend** | Đọc cử chỉ → Queue1 |
| `defaultTask` | `StartDefaultTask` | Chạy | Tiêu thụ Queue2, điều phối Task03/Task04, điều khiển LED |

`myTask04` bị `osThreadSuspend()` ngay sau khi tạo, khớp với chế độ mặc định `currentMode = MODE_TASK3` (nút bấm). Nhờ vậy khi khởi động, bus I²C không bị poll vô ích và chỉ đúng một nguồn điều khiển hoạt động.

### Bất biến quan trọng

> **Tại mọi thời điểm, đúng một trong hai task Task03/Task04 đang chạy.**

Bất biến này được `updateTasksBasedOnMode()` duy trì. Nó là lý do hai task có thể cùng ghi vào Queue1 mà không cần mutex: chúng không bao giờ chạy đồng thời.

---

## 4. Message queue

### Queue1 — lệnh điều khiển

```c
Queue1Handle = osMessageQueueNew(8, sizeof(uint8_t), &Queue1_Attributes);
```

Sức chứa 8 phần tử, mỗi phần tử 1 byte. Lệnh mã hoá bằng ký tự ASCII:

| Byte | Ý nghĩa | Sinh bởi |
|:--:|---|---|
| `'L'` | Di chuyển trái | Nút PG2 · cử chỉ vuốt trái |
| `'R'` | Di chuyển phải | Nút PG3 · cử chỉ vuốt phải |
| `'D'` | Rơi nhanh | Nút PB12 · cử chỉ vuốt xuống |
| `'C'` | Xoay khối | Nút PB13 · cử chỉ xoay theo chiều kim đồng hồ |

Byte `'O'` được dùng nội bộ trong task làm giá trị "không có lệnh" và **không bao giờ** được đẩy vào queue.

### Queue2 — chế độ điều khiển

```c
Queue2Handle = osMessageQueueNew(8, sizeof(uint8_t), &Queue2_Attributes);
```

| Byte | Chế độ | Task hoạt động | LED |
|:--:|---|---|---|
| `0` | Nút bấm (`MODE_TASK3`) | `myTask03` | PG13 xanh lá |
| `1` | Cử chỉ (`MODE_TASK4`) | `myTask04` | PG14 đỏ |

Giá trị được ánh xạ sang enum `SystemMode` trong `StartDefaultTask`:

```c
currentMode = mode == 0 ? MODE_TASK3 : MODE_TASK4;
```

Lưu ý: mọi giá trị khác `0` đều rơi vào `MODE_TASK4`. Đây là hành vi cố ý (chỉ có hai chế độ), không phải thiếu sót kiểm tra.

---

## 5. Cơ chế chống lặp lệnh

Cả `StartTask03` và `StartTask04` dùng chung một mẫu:

```c
uint32_t count = osMessageQueueGetCount(Queue1Handle);
if (count < 1) {
  if (/* có lệnh hợp lệ */) {
    osMessageQueuePut(Queue1Handle, &msg, 0, 10);
    osDelay(400);          // cửa sổ khoá — chặn lệnh lặp
  }
}
osDelay(10);               // chu kỳ quét
```

Hai lớp bảo vệ hoạt động cùng lúc:

1. **`count < 1`** — không đẩy lệnh mới khi game chưa tiêu thụ lệnh cũ. Ngăn queue tích tụ lệnh tồn đọng khiến khối gạch "trôi" sau khi người chơi đã thả tay.
2. **`osDelay(400)`** — sau mỗi lệnh thành công, task ngủ 400 ms. Đây vừa là chống dội nút (debounce), vừa giới hạn tốc độ lặp ở khoảng 2,5 lệnh/giây.

Dù `Queue1` khai báo 8 phần tử, bất biến `count < 1` khiến trên thực tế nó **không bao giờ chứa quá 1 phần tử**. Sức chứa dư là biên an toàn, không phải buffer thực sự.

> **Muốn game nhạy hơn?** Giảm `osDelay(400)`. Giá trị quanh 150–200 ms cho cảm giác phản hồi tốt hơn với nút bấm, nhưng với cử chỉ thì dễ khiến một cái vuốt tay bị đọc thành nhiều lệnh.

---

## 6. Chuyển chế độ

Khi người chơi chọn chế độ trong menu, GUI đẩy `0` hoặc `1` vào Queue2. `StartDefaultTask` poll queue mỗi 1 ms:

```c
uint32_t count = osMessageQueueGetCount(Queue2Handle);
if (count > 0) {
  uint8_t mode;
  osMessageQueueGet(Queue2Handle, &mode, NULL, 0);
  currentMode = mode == 0 ? MODE_TASK3 : MODE_TASK4;
  /* … bật LED tương ứng … */
  updateTasksBasedOnMode();
}
osDelay(1);
```

`updateTasksBasedOnMode()` resume task cần thiết và suspend task còn lại:

```c
case MODE_TASK3:
  if (osThreadGetState(myTask03Handle) == osThreadBlocked) osThreadResume(myTask03Handle);
  if (osThreadGetState(myTask04Handle) != osThreadBlocked) osThreadSuspend(myTask04Handle);
  break;
```

Việc kiểm tra `osThreadGetState()` trước khi gọi resume/suspend làm hàm này **idempotent** — gọi nhiều lần với cùng một chế độ không gây tác dụng phụ.

> **Chi tiết đáng lưu ý:** hàm so sánh với `osThreadBlocked` chứ không phải `osThreadInactive`. Task đang ngủ trong `osDelay()` cũng ở trạng thái blocked, nên phép kiểm tra này gộp chung "đang bị suspend" và "đang ngủ giữa hai lần quét". Với luồng chạy hiện tại điều đó vô hại, vì resume một task đang `osDelay` chỉ đơn giản đánh thức nó sớm.

---

## 7. Driver PAJ7620

[`Core/Src/PAJ7620.c`](../Core/Src/PAJ7620.c) hiện thực driver cho cảm biến cử chỉ PAJ7620U2 qua I²C3, địa chỉ 7-bit `0x73`.

Driver decode đủ 9 cử chỉ:

```c
GES_RIGHT, GES_LEFT, GES_UP, GES_DOWN,
GES_FORWARD, GES_BACKWARD,
GES_CLOCKWISE, GES_ANTICLOCKWISE, GES_WAVE
```

`StartTask04` ánh xạ chúng sang byte (`'R'`, `'L'`, `'U'`, `'D'`, `'F'`, `'B'`, `'C'`, `'A'`), nhưng **chỉ bốn** byte được đẩy vào Queue1:

```c
if (data == 'R' || data == 'L' || data == 'D' || data == 'C') { … }
```

Các cử chỉ còn lại bị bỏ qua một cách im lặng. Việc decode đầy đủ được giữ lại để dễ mở rộng — muốn thêm chức năng "thả khối ngay" bằng cử chỉ *forward* chỉ cần nới điều kiện lọc này.

---

## 8. Đồ hoạ

| Thành phần | Vai trò |
|---|---|
| **LTDC** | Bộ điều khiển LCD, quét framebuffer ra màn hình ILI9341 240×320 |
| **DMA2D** | Tăng tốc phần cứng cho fill và blit, giải phóng CPU khỏi việc vẽ |
| **FMC / SDRAM** | Chứa framebuffer — quá lớn so với SRAM nội của MCU |
| **TouchGFX** | Framework GUI, chạy trong `GUI_Task` |

Luồng màn hình:

```
ScreenWait ──▶ Screen chọn chế độ ──▶ Screen chơi Tetris
```

Mỗi màn hình theo mẫu **Model-View-Presenter** của TouchGFX: `View` lo hiển thị, `Presenter` làm cầu nối, `Model` giữ trạng thái dùng chung giữa các màn hình.

---

## 9. Cấu hình clock

Thiết lập trong `SystemClock_Config()`:

| Thông số | Giá trị |
|---|---|
| Nguồn dao động | HSE 8 MHz (thạch anh trên board) |
| PLL | M = 8, N = 360, P = 2, Q = 4 |
| SYSCLK | **180 MHz** |
| HCLK | 180 MHz |
| APB1 | 45 MHz (HCLK / 4) |
| APB2 | 90 MHz (HCLK / 2) |
| Flash latency | 5 wait states |
| Timebase HAL | TIM6 |

HAL dùng **TIM6** thay vì SysTick ([`Core/Src/stm32f4xx_hal_timebase_tim.c`](../Core/Src/stm32f4xx_hal_timebase_tim.c)) vì FreeRTOS đã chiếm SysTick làm tick nguồn của scheduler.

---

## 10. Hướng mở rộng

| Ý tưởng | Cần sửa ở đâu |
|---|---|
| Thêm nguồn điều khiển (UART, accelerometer) | Tạo task mới đẩy vào Queue1; thêm nhánh vào `updateTasksBasedOnMode()` và enum `SystemMode` |
| Dùng thêm cử chỉ của PAJ7620 | Nới điều kiện lọc trong `StartTask04`, thêm mã lệnh mới vào Queue1 |
| Điều khiển bằng cảm ứng | LCD có sẵn STMPE811 (driver đã có trong `Drivers/BSP/`) — TouchGFX xử lý sự kiện chạm trực tiếp, không cần qua Queue1 |
| Thay poll bằng interrupt | Chuyển nút sang EXTI, đẩy lệnh từ ISR bằng `osMessageQueuePut(..., 0)` — lưu ý timeout phải là 0 khi gọi từ ISR |
| Lưu điểm cao | Ghi vào Flash bằng HAL FLASH, hoặc backup SRAM |
