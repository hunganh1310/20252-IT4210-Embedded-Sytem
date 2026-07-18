#include <gui/screen3_screen/Screen3View.hpp>

// Địa hình LEVEL 1: 8 khối chướng ngại xếp sẵn ở đáy bàn chơi.
// {pixelX, pixelY, biến thể trong tetris::TETROMINOS_INIT}. Toạ độ phải nằm đúng lưới.
const tetris::InitBlock Screen3View::INIT_BLOCKS[NUM_FIXED_BLOCKS] = {
    {  20, 250,  1 },
    {  35, 265, 12 },
    {  50, 250, 13 },
    {  80, 280, 17 },
    {  95, 250, 10 },
    { 125, 280, 17 },
    { 125, 235,  3 },
    { 140, 220,  9 }
};

Screen3View::Screen3View()
{
    startGame();

    // Hiển thị mốc điểm mục tiêu (widget riêng của màn level, không có ở chế độ endless).
    // Sau startGame(), targetScore đã bằng targetScoreValue().
    touchgfx::Unicode::snprintf(textArea3Buffer, sizeof(textArea3Buffer), "%d", targetScore);
    textArea3.setWildcard(textArea3Buffer);
    textArea3.setTypedText(touchgfx::TypedText(T_TARGET3));
    textArea3.invalidate();
}

// Bảng "qua màn": engine đã dọn sân và invalidate, ở đây chỉ bật/ẩn các widget tương ứng.
void Screen3View::showWinScreen()
{
    image3.setVisible(true);       // Bảng chúc mừng qua màn
    flexButton1.setVisible(false); // Ẩn nút điều khiển đang chơi
    flexButton4.setVisible(true);
    flexButton5.setVisible(true);
}
