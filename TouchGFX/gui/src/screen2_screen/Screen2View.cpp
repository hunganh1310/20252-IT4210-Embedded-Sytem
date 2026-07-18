#include <gui/screen2_screen/Screen2View.hpp>
#include "cmsis_os2.h"
#include "main.h" 

// Hàng đợi chọn chế độ điều khiển, đọc bởi StartDefaultTask trong main.c.
// Giá trị 0 = điều khiển bằng nút bấm (Task03), 1 = điều khiển bằng cử chỉ tay (Task04).
extern osMessageQueueId_t Queue2Handle;

Screen2View::Screen2View()
{
    tickCount = 0;
}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();
}

void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
}

// Chọn chế độ điều khiển bằng NÚT BẤM.
// Được gọi từ interaction của nút trong TouchGFX Designer; chỉ gửi yêu cầu qua queue,
// việc bật/tắt task do StartDefaultTask bên main.c thực hiện.
void Screen2View::turnOnMode1()
{
    // Implementation for turning on mode 1
    uint8_t mode = 0;
    uint32_t count = osMessageQueueGetCount(Queue2Handle);
    // Chỉ gửi khi queue rỗng để tránh dồn lệnh nếu người dùng bấm nút liên tục
    if (count < 1)
    {
        osMessageQueuePut(Queue2Handle, &mode, 0, 10);
    }
}

// Chọn chế độ điều khiển bằng CỬ CHỈ TAY (cảm biến PAJ7620).
void Screen2View::turnOnMode2()
{
    // Implementation for turning on mode 2
    uint8_t mode = 1;
    uint32_t count = osMessageQueueGetCount(Queue2Handle);
    if (count < 1)
    {
        osMessageQueuePut(Queue2Handle, &mode, 0, 10);
    }
}

// Hiệu ứng động của màn hình chọn chế độ: 3 ảnh lần lượt biến mất rồi lần lượt hiện lại,
// tạo vòng lặp nhấp nháy chu kỳ 18 tick. Mỗi pha cách nhau 3 tick.
void Screen2View::handleTickEvent()
{
    tickCount++;
    switch(tickCount % 18) {
        case 0:  // Phase 1 - First image disappears
            image6.setVisible(false);
            image7.setVisible(true);
            image5.setVisible(true);
            break;
            
        case 3:  // Phase 2 - Second image disappears
            image6.setVisible(false);
            image7.setVisible(false);
            image5.setVisible(true);
            break;
            
        case 6:  // Phase 3 - Third image disappears
            image6.setVisible(false);
            image7.setVisible(false);
            image5.setVisible(false);
            break;
            
        case 9:  // Phase 4 - First image appears
            image6.setVisible(true);
            image7.setVisible(false);
            image5.setVisible(false);
            break;
            
        case 12:  // Phase 5 - Second image appears
            image6.setVisible(true);
            image7.setVisible(true);
            image5.setVisible(false);
            break;
            
        case 15:  // Phase 6 - Third image appears
            image6.setVisible(true);
            image7.setVisible(true);
            image5.setVisible(true);
            break;
    }
    invalidate();
}
