# Hướng dẫn đóng góp

Tài liệu dành cho thành viên nhóm đồ án IT4210 — 20252, và cho bất kỳ ai muốn mở rộng dự án.

---

## Chuẩn bị môi trường

| Công cụ | Phiên bản | Bắt buộc? |
|---|---|---|
| **STM32CubeIDE** | ≥ 1.17.0 | Có (hoặc IAR / Keil / arm-none-eabi-gcc) |
| **TouchGFX Designer** | 4.25.0 | Chỉ khi sửa giao diện |
| **STM32CubeMX** | ≥ 6.13 | Chỉ khi sửa cấu hình ngoại vi (đã tích hợp trong CubeIDE) |
| **Git** | bất kỳ | Có |

> Phiên bản TouchGFX Designer phải **đúng 4.25.0**. Mở project bằng bản khác, Designer sẽ tự nâng cấp file `.touchgfx` và sinh ra code không tương thích với thư viện trong `Middlewares/`.

---

## Quy tắc quan trọng nhất: khối USER CODE

Phần lớn code trong `Core/` do **STM32CubeMX sinh tự động**. Mỗi lần ai đó mở `.ioc` và bấm *Generate Code*, CubeMX ghi đè các file này và **chỉ giữ lại** phần nằm giữa các dấu mốc:

```c
/* USER CODE BEGIN 2 */
   // code viết ở đây được giữ lại
/* USER CODE END 2 */

   // code viết ở đây SẼ BỊ XOÁ
```

**Luôn viết code trong các khối `USER CODE`.** Đây là nguyên nhân mất code phổ biến nhất trong các dự án STM32.

Tương tự với TouchGFX: `TouchGFX/generated/` do Designer sinh ra — đừng sửa tay. Code của nhóm nằm ở `TouchGFX/gui/`.

---

## Style code

Repo có [`.editorconfig`](.editorconfig); hầu hết editor sẽ tự áp dụng.

- **Thụt lề:** 2 space (Makefile dùng tab)
- **Độ dài dòng:** tối đa 120 ký tự
- **Ngoặc nhọn:** theo style của code CubeMX sinh ra — mở ngoặc ở dòng mới cho hàm, cùng dòng cho khối điều khiển
- **Đặt tên:**
  - Hàm task: `StartTaskXX` (giữ đúng tên CubeMX sinh)
  - Hàm helper: `camelCase` — ví dụ `updateTasksBasedOnMode()`
  - Hằng số & macro: `UPPER_SNAKE_CASE`
  - Driver PAJ7620: tiền tố `PAJ7620_`
- **Comment:** tiếng Việt hoặc tiếng Anh đều được, nhưng **thống nhất trong một file**. Ưu tiên giải thích *tại sao*, không mô tả *cái gì* — dòng code đã tự nói lên điều đó.

---

## Commit

Dùng [Conventional Commits](https://www.conventionalcommits.org/):

```
<type>(<scope>): <mô tả ngắn>
```

| Type | Dùng khi |
|---|---|
| `feat` | Thêm tính năng |
| `fix` | Sửa lỗi |
| `refactor` | Đổi cấu trúc, không đổi hành vi |
| `docs` | Chỉ sửa tài liệu |
| `chore` | Build, gitignore, công cụ |

Scope gợi ý: `core`, `gui`, `paj7620`, `rtos`, `docs`, `build`.

```
feat(paj7620): thêm cử chỉ forward để thả khối ngay
fix(rtos): sửa race khi chuyển chế độ lúc queue chưa rỗng
docs(hardware): bổ sung sơ đồ điện trở kéo xuống cho nút bấm
```

### Trước khi commit — kiểm tra phạm vi thay đổi

```bash
git status
git diff --stat
```

Dự án này **đã từng mất toàn bộ thư mục `TouchGFX/`** vì một commit khai là "xoá vài file generated" nhưng thực tế xoá 182 file (xem [CHANGELOG](CHANGELOG.md)). Nếu `--stat` hiện nhiều file hơn bạn nghĩ, dừng lại và tìm hiểu tại sao trước khi commit.

Cụ thể, cẩn thận khi:
- Chạy *Generate Code* — CubeMX/Designer có thể xoá file bạn không ngờ tới
- Dọn thư mục build bằng tay
- `git add .` sau khi IDE vừa chạy clean

---

## Nhánh

- `main` — luôn phải build được
- `feat/<tên>` — tính năng mới
- `fix/<tên>` — sửa lỗi

Merge vào `main` qua Pull Request, có ít nhất một người trong nhóm review.

---

## Kiểm thử

Dự án chưa có test tự động (firmware, cần phần cứng thật). Trước khi merge, tự kiểm tra trên board theo danh sách trong [docs/HARDWARE.md § 8](docs/HARDWARE.md#8-kiểm-tra--sự-cố-thường-gặp):

- [ ] Build sạch, không warning mới
- [ ] Khởi động: LED xanh PG13 sáng
- [ ] Cả 4 nút phản hồi đúng
- [ ] Chuyển sang chế độ cử chỉ: LED chuyển sang đỏ PG14
- [ ] Cả 4 cử chỉ phản hồi đúng
- [ ] Chuyển qua lại nhiều lần: không treo, không lệnh ma

Trong mô tả PR, ghi rõ bạn đã kiểm tra trên phần cứng hay chưa.

---

## Sửa giao diện

1. Mở `TouchGFX/MyApplication_2.touchgfx` bằng TouchGFX Designer **4.25.0**
2. Sửa
3. Bấm **Generate Code** — bước này bắt buộc, nếu không code sẽ không khớp với thiết kế
4. Build lại từ IDE
5. Commit **cả** file `.touchgfx` **và** thư mục `generated/` đi kèm

---

## Thêm nguồn điều khiển mới

Kiến trúc đã được thiết kế cho việc này (xem [ARCHITECTURE.md § 10](docs/ARCHITECTURE.md#10-hướng-mở-rộng)):

1. Thêm giá trị vào enum `SystemMode` trong `main.c`
2. Tạo task mới, đẩy byte `'L'`/`'R'`/`'D'`/`'C'` vào `Queue1Handle`
3. Thêm nhánh `case` vào `updateTasksBasedOnMode()` — nhớ suspend các task khác
4. Cập nhật GUI để gửi giá trị chế độ mới vào `Queue2Handle`

**Không sửa logic game.** Nếu bạn thấy mình phải sửa nó để thêm nguồn điều khiển, nghĩa là lớp trừu tượng đã bị phá vỡ ở đâu đó.
