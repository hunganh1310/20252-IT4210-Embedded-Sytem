# Phần cứng

Hướng dẫn đấu nối và cấu hình ngoại vi cho dự án. Xem [ARCHITECTURE.md](ARCHITECTURE.md) cho phần thiết kế phần mềm.

---

## 1. Danh sách linh kiện

| Linh kiện | Số lượng | Ghi chú |
|---|:--:|---|
| **STM32F429I-DISCO** | 1 | MCU STM32F429ZIT6; LCD 240×320 và SDRAM có sẵn trên board |
| **Cảm biến cử chỉ PAJ7620U2** | 1 | Module I²C, thường bán dưới tên "Grove Gesture" hoặc "PAJ7620U2 module" |
| **Nút bấm tact switch** | 4 | Loại 4 chân thông dụng |
| **Điện trở 10 kΩ** | 4 | Kéo xuống (pull-down) cho các nút |
| **Dây nối** | ~10 | Đực–cái để nối từ header của board sang breadboard |
| **Breadboard** | 1 | |
| **Cáp micro-USB** | 1 | Cấp nguồn + ST-Link tích hợp trên board |

---

## 2. Board STM32F429I-DISCO

Board đã tích hợp sẵn phần lớn ngoại vi mà dự án cần — không phải nối thêm gì cho màn hình:

| Ngoại vi trên board | Kết nối | Dự án dùng để |
|---|---|---|
| LCD 240×320 (ILI9341) | LTDC + SPI5 | Hiển thị game |
| SDRAM 64 Mb (IS42S16400J) | FMC | Chứa framebuffer |
| Cảm ứng (STMPE811) | I²C3 | *Không dùng* (driver có sẵn) |
| LED xanh lá LD3 | **PG13** | Báo chế độ nút bấm |
| LED đỏ LD4 | **PG14** | Báo chế độ cử chỉ |
| ST-Link/V2-B | USB | Nạp & debug |

> **Lưu ý về I²C3:** cảm ứng STMPE811 trên board dùng chung bus I²C3 với PAJ7620, nhưng địa chỉ khác nhau (`0x41` so với `0x73`) nên hai thiết bị chung sống được. Dự án không khởi tạo STMPE811.

---

## 3. Cảm biến cử chỉ PAJ7620

### Đấu nối

| Chân sensor | Chân MCU | Header trên board | Chức năng |
|---|---|---|---|
| **VCC** | 3.3 V | P1 pin 1 hoặc P2 | Nguồn — **không dùng 5 V** |
| **GND** | GND | P1 / P2 | Mát chung |
| **SCL** | **PA8** | P1 | `I2C3_SCL`, AF4 |
| **SDA** | **PC9** | P1 | `I2C3_SDA`, AF4 |

```
   PAJ7620 module                  STM32F429I-DISCO
   ┌──────────────┐
   │  VCC  ───────┼──────────────▶ 3.3 V
   │  GND  ───────┼──────────────▶ GND
   │  SCL  ───────┼──────────────▶ PA8   (I2C3_SCL)
   │  SDA  ───────┼──────────────▶ PC9   (I2C3_SDA)
   └──────────────┘
            ▲
            └── phần lớn module đã có sẵn điện trở kéo lên 4.7 kΩ trên bo.
                Nếu module của bạn không có, thêm 4.7 kΩ từ SCL và SDA lên 3.3 V.
```

### Thông số

| Thông số | Giá trị |
|---|---|
| Địa chỉ I²C (7-bit) | `0x73` |
| Địa chỉ dùng trong HAL (8-bit) | `0xE6` (`0x73 << 1`) |
| Điện áp hoạt động | 2.8 – 3.3 V |
| Khoảng cách nhận cử chỉ | ~5 – 15 cm |
| Bus | I²C3, chế độ Standard |

Định nghĩa trong [`Core/Inc/PAJ7620.h`](../Core/Inc/PAJ7620.h):

```c
#define PAJ7620_I2C_BUS_ADDR   0x73 << 1   // HAL cần địa chỉ 8-bit
```

### Cử chỉ nhận được

Driver decode 9 cử chỉ; game dùng 4:

| Cử chỉ | Enum | Game dùng? |
|---|---|:--:|
| Vuốt trái | `GES_LEFT` | ✅ → `'L'` |
| Vuốt phải | `GES_RIGHT` | ✅ → `'R'` |
| Vuốt xuống | `GES_DOWN` | ✅ → `'D'` |
| Xoay thuận chiều kim đồng hồ | `GES_CLOCKWISE` | ✅ → `'C'` |
| Vuốt lên | `GES_UP` | ❌ |
| Đưa tay lại gần | `GES_FORWARD` | ❌ |
| Đưa tay ra xa | `GES_BACKWARD` | ❌ |
| Xoay ngược chiều kim đồng hồ | `GES_ANTICLOCKWISE` | ❌ |
| Vẫy tay | `GES_WAVE` | ❌ |

---

## 4. Nút bấm

### Cấu hình — active-HIGH

Tất cả nút đều **active-HIGH**: nhấn nút → chân MCU đọc mức `1`. Điều này bắt buộc phải có **điện trở kéo xuống**, nếu không chân sẽ thả nổi và game sẽ nhận lệnh ma.

```
        3.3 V
          │
          ├──────○  ○──────┬──────────▶ chân MCU (PG2 / PG3 / PB12 / PB13)
          │   nút bấm      │
                           │
                          ┌┴┐
                          │ │ 10 kΩ  (kéo xuống)
                          └┬┘
                           │
                          GND
```

Không nhấn → chân bị 10 kΩ kéo về `0`. Nhấn → chân nối thẳng lên 3.3 V, đọc được `1`.

> Thay vì điện trở rời, có thể bật pull-down nội của MCU bằng cách đặt `GPIO_InitStruct.Pull = GPIO_PULLDOWN` trong `MX_GPIO_Init()`. Điện trở rời cho mức nghỉ chắc chắn hơn khi dây dài trên breadboard.

### Bảng chân

| Nút | Chân MCU | Port | Lệnh gửi | Hành động trong game |
|---|---|---|:--:|---|
| Trái | **PG2** | GPIOG | `'L'` | Di chuyển khối sang trái |
| Phải | **PG3** | GPIOG | `'R'` | Di chuyển khối sang phải |
| Xuống | **PB12** | GPIOB | `'D'` | Rơi nhanh |
| Xoay | **PB13** | GPIOB | `'C'` | Xoay khối |

Được đọc trong `StartTask03` ([`Core/Src/main.c`](../Core/Src/main.c)):

```c
if (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_2) == GPIO_PIN_SET) msg = 'L';
```

> **Đừng nhầm chân:** [`Core/Inc/main.h`](../Core/Inc/main.h) có định nghĩa `VSYNC_FREQ_Pin` = `GPIO_PIN_2` và `RENDER_TIME_Pin` = `GPIO_PIN_3` — trùng *số* chân với nút trái và nút phải, nhưng chúng nằm trên **GPIOE** (`VSYNC_FREQ_GPIO_Port = GPIOE`), là chân đo hiệu năng của TouchGFX. Không xung đột với nút bấm trên GPIOG.

---

## 5. LED báo chế độ

Dùng hai LED có sẵn trên board, không cần nối thêm:

| LED | Chân | Màu | Sáng khi |
|---|---|---|---|
| LD3 | **PG13** | Xanh lá | Chế độ **nút bấm** (`MODE_TASK3`) |
| LD4 | **PG14** | Đỏ | Chế độ **cử chỉ** (`MODE_TASK4`) |

Hai LED loại trừ lẫn nhau — luôn đúng một LED sáng, phản ánh trực tiếp bất biến "đúng một task điều khiển đang chạy". Đây là cách kiểm tra nhanh nhất xem việc chuyển chế độ có hoạt động không.

---

## 6. Cấu hình ngoại vi

Toàn bộ cấu hình do **STM32CubeMX** sinh ra từ [`STM32F429I_DISCO_REV_D01.ioc`](../STM32F429I_DISCO_REV_D01.ioc). Mở file này bằng CubeMX (hoặc trong STM32CubeIDE) để xem/sửa bằng giao diện.

| Ngoại vi | Cấu hình | Dùng cho |
|---|---|---|
| **RCC** | HSE 8 MHz → PLL(M=8, N=360, P=2) → 180 MHz | Clock hệ thống |
| **I2C3** | PA8 (SCL), PC9 (SDA), AF4 | PAJ7620 |
| **GPIOG** | PG2, PG3 input · PG13, PG14 output | Nút bấm, LED |
| **GPIOB** | PB12, PB13 input | Nút bấm |
| **LTDC** | RGB565, 240×320 | Màn hình LCD |
| **DMA2D** | Register-to-memory | Tăng tốc đồ hoạ |
| **FMC** | SDRAM Bank 1, CAS latency 3 | Framebuffer |
| **SPI5** | NCS = PC1 | Lệnh điều khiển ILI9341 |
| **TIM6** | Timebase HAL | `HAL_IncTick()` |
| **CRC** | — | TouchGFX yêu cầu |

> Sau khi sửa `.ioc` và **Generate Code**, CubeMX chỉ giữ lại phần nằm trong các khối `/* USER CODE BEGIN */ … /* USER CODE END */`. Mọi code viết ngoài các khối đó **sẽ bị xoá**.

---

## 7. Quy trình lắp ráp

1. **Cắm board qua micro-USB** — LED nguồn sáng, xác nhận board hoạt động.
2. **Lắp 4 nút bấm lên breadboard**, mỗi nút kèm một điện trở 10 kΩ xuống GND theo sơ đồ ở mục 4.
3. **Nối chân tín hiệu nút** → PG2, PG3, PB12, PB13.
4. **Nối module PAJ7620** → 3.3 V, GND, PA8 (SCL), PC9 (SDA).
5. **Kiểm tra lại nguồn trước khi cấp điện** — nối nhầm VCC của sensor vào 5 V có thể làm hỏng cảm biến.
6. **Nạp firmware** và kiểm tra: LED xanh PG13 phải sáng ngay khi khởi động (chế độ nút bấm mặc định).

---

## 8. Kiểm tra & sự cố thường gặp

### Danh sách kiểm tra nhanh

| Kiểm tra | Kết quả mong đợi |
|---|---|
| Cấp nguồn board | LED nguồn sáng, màn hình hiện ScreenWait |
| Khởi động xong | LED xanh **PG13** sáng, LED đỏ PG14 tắt |
| Chọn chế độ cử chỉ trong menu | LED đổi sang đỏ **PG14** |
| Nhấn từng nút ở chế độ nút bấm | Khối gạch phản hồi đúng hướng |
| Vuốt tay cách sensor ~10 cm | Khối gạch phản hồi đúng hướng |

### Sự cố

| Hiện tượng | Nguyên nhân thường gặp |
|---|---|
| **Không nhận cử chỉ nào** | Sai dây SCL/SDA (dễ đảo nhầm PA8 ↔ PC9); thiếu điện trở kéo lên; sensor cấp 5 V thay vì 3.3 V |
| **Cử chỉ chập chờn** | Tay quá gần (< 5 cm) hoặc quá xa (> 15 cm); ánh sáng hồng ngoại mạnh chiếu vào (nắng trực tiếp) |
| **Nút tự bấm liên tục** | Thiếu điện trở kéo xuống → chân thả nổi |
| **Nút không phản hồi** | Nối nhầm sang GND (nút là active-HIGH, phải nối lên 3.3 V) |
| **Chỉ một nút hoạt động** | Trong `StartTask03`, các lệnh `if` chạy tuần tự nên nút sau ghi đè nút trước — đây là hành vi cố ý: nhấn nhiều nút cùng lúc thì `'C'` (PB13) thắng |
| **LED không đổi khi chuyển chế độ** | `defaultTask` không nhận được Queue2 — kiểm tra GUI có gọi `osMessageQueuePut(Queue2Handle, …)` không |
| **Màn hình đen** | SDRAM chưa init — kiểm tra `MX_FMC_Init()` và CAS latency |
| **Không nạp được** | Cáp USB chỉ có dây nguồn (không có dây data); thử cáp khác |
