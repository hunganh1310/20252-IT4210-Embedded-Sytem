# 🎮 STM32F429I Tetris — Điều khiển bằng Nút bấm & Cử chỉ

Game Tetris chạy trên board **STM32F429I Discovery**, giao diện dựng bằng **TouchGFX**, chơi được bằng **nút bấm vật lý** hoặc **cảm biến cử chỉ PAJ7620**. Toàn bộ hệ thống chạy đa nhiệm trên **FreeRTOS (CMSIS-RTOS v2)**.

> Đồ án môn học **IT4210 — Hệ thống nhúng (Embedded System)**, học kỳ 20252.

---

## ✨ Tính năng

- 🕹️ **Hai nguồn điều khiển:** nút bấm vật lý hoặc cử chỉ tay (chọn từ menu).
- 👋 **Cảm biến cử chỉ PAJ7620** giao tiếp qua I²C.
- 🎨 **Giao diện TouchGFX** trên màn hình LCD 240×320 (ILI9341 + LTDC + DMA2D).
- ⚡ **Đa nhiệm FreeRTOS:** tách riêng task GUI, task đọc nút, task đọc cử chỉ.
- 🔀 **Chuyển chế độ nóng:** đổi giữa điều khiển nút/cử chỉ ngay khi đang chạy; LED trên board báo chế độ hiện tại.

---

## 🧩 Kiến trúc phần mềm

Các task giao tiếp với nhau qua hai message queue của FreeRTOS:

| Thành phần        | Vai trò                                                                 |
|-------------------|-------------------------------------------------------------------------|
| `GUI_Task`        | Chạy TouchGFX, vẽ màn hình và xử lý logic Tetris.                        |
| `myTask03`        | Đọc 4 nút bấm, đẩy lệnh vào **Queue1**.                                  |
| `myTask04`        | Đọc cử chỉ từ PAJ7620, đẩy lệnh vào **Queue1**.                          |
| `defaultTask`     | Nhận chế độ từ **Queue2**, tạm dừng/khôi phục Task03 hoặc Task04, bật LED báo chế độ. |

- **Queue1** — hàng đợi lệnh điều khiển, mã hoá bằng ký tự: `L` (trái), `R` (phải), `D` (rơi nhanh), `C` (xoay). Màn hình game đọc từ đây nên logic game **không phụ thuộc** vào nguồn điều khiển.
- **Queue2** — chọn chế độ điều khiển: `0` = nút bấm (Task03), `1` = cử chỉ (Task04).

Luồng màn hình TouchGFX: **màn hình chờ → menu chọn chế độ → màn hình chơi Tetris**.

---

## 🔧 Yêu cầu

### Phần cứng
- **STM32F429I-DISCO** (MCU STM32F429ZIT6, có sẵn LCD & SDRAM trên board).
- **Cảm biến cử chỉ PAJ7620** (I²C).
- **4 nút bấm** + điện trở kéo phù hợp và dây nối.

### Phần mềm
- **STM32CubeIDE** ≥ 1.17.0 (hoặc EWARM / MDK-ARM — có sẵn project cho cả ba).
- **TouchGFX Designer** 4.25.0 (chỉ cần khi chỉnh sửa giao diện).

---

## 🔌 Sơ đồ kết nối

### Cảm biến PAJ7620 (I²C3)
| Chân sensor | Chân MCU |
|-------------|----------|
| SCL         | PA8      |
| SDA         | PC9      |
| VCC         | 3.3V     |
| GND         | GND      |

### Nút bấm
| Nút    | Chân MCU | Lệnh          |
|--------|----------|---------------|
| Trái   | PG2      | `L` Di chuyển trái |
| Phải   | PG3      | `R` Di chuyển phải |
| Xuống  | PB12     | `D` Rơi nhanh |
| Xoay   | PB13     | `C` Xoay khối |

---

## 🚀 Cài đặt & chạy

```bash
git clone <repository-url>
```

1. Mở project bằng **STM32CubeIDE** (thư mục `STM32CubeIDE/`).
2. Build: `Project → Build Project` (hoặc `Ctrl+B`).
3. Đấu nối phần cứng theo sơ đồ trên.
4. Nạp và chạy: `Run → Debug` / `Run As → STM32 C/C++ Application`.

> Cũng có thể build bằng **EWARM** (`EWARM/Project.eww`), **MDK-ARM** (`MDK-ARM/*.uvprojx`) hoặc dòng lệnh `make` trong thư mục `gcc/`.

---

## 🎮 Điều khiển

### Cử chỉ (PAJ7620)
| Cử chỉ         | Hành động       |
|----------------|-----------------|
| Vuốt trái      | Di chuyển trái  |
| Vuốt phải      | Di chuyển phải  |
| Vuốt xuống     | Rơi nhanh       |
| Xoay theo chiều kim đồng hồ | Xoay khối |

### Nút bấm
PG2 = trái · PG3 = phải · PB12 = rơi nhanh · PB13 = xoay.

Đổi giữa hai chế độ điều khiển ngay trong **menu chọn chế độ**.

---

## 📁 Cấu trúc thư mục

```
├── Core/            # main.c, freertos.c, driver PAJ7620, cấu hình HAL
│   ├── Src/         #   mã nguồn ứng dụng & khởi tạo ngoại vi
│   └── Inc/         #   header, FreeRTOSConfig.h
├── TouchGFX/        # Project & mã giao diện TouchGFX
│   └── gui/src/     #   View/Presenter các màn hình (menu, game, chờ)
├── Drivers/         # STM32 HAL, CMSIS, BSP
├── Middlewares/     # TouchGFX, FreeRTOS
├── STM32CubeIDE/    # Project STM32CubeIDE
├── EWARM/           # Project IAR EWARM
├── MDK-ARM/         # Project Keil uVision
├── gcc/             # Makefile build bằng arm-none-eabi-gcc
└── STM32F429I_DISCO_REV_D01.ioc  # Cấu hình STM32CubeMX
```

---

## 🐛 Xử lý sự cố

| Hiện tượng          | Cách xử lý                                                        |
|---------------------|------------------------------------------------------------------|
| Lỗi build TouchGFX  | Mở file `.touchgfx` và **Generate Code** lại từ TouchGFX Designer. |
| Không nạp được      | Kiểm tra kết nối ST-Link và nguồn cấp cho board.                  |
| Cử chỉ không nhận   | Kiểm tra dây I²C (PA8/PC9) và địa chỉ/khởi tạo PAJ7620.           |
| Nút không phản hồi  | Kiểm tra chân PG2/PG3/PB12/PB13 và điện trở kéo.                  |
