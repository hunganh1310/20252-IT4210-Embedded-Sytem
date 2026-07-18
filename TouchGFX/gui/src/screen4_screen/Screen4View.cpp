#include <gui/screen4_screen/Screen4View.hpp>

// Địa hình LEVEL 2: 8 khối chướng ngại, xếp gồ ghề hơn level 1.
// {pixelX, pixelY, biến thể trong tetris::TETROMINOS_INIT}. Toạ độ phải nằm đúng lưới.
const tetris::InitBlock Screen4View::INIT_BLOCKS[NUM_FIXED_BLOCKS] = {
    {  20, 265,  3 },
    {  35, 235,  6 },
    {  50, 265, 11 },
    {  65, 280,  0 },
    {  95, 235, 16 },
    { 110, 280, 15 },
    { 125, 205,  7 },
    { 140, 250, 12 }
};

Screen4View::Screen4View()
{
    startGame();

    // Hiển thị mốc điểm mục tiêu (widget riêng của màn level, không có ở chế độ endless).
    // Sau startGame(), targetScore đã bằng targetScoreValue().
    touchgfx::Unicode::snprintf(textArea3Buffer, sizeof(textArea3Buffer), "%d", targetScore);
    textArea3.setWildcard(textArea3Buffer);
    textArea3.setTypedText(touchgfx::TypedText(T_TARGET4));
    textArea3.invalidate();
}

// Bảng "qua màn": engine đã dọn sân và invalidate, ở đây chỉ bật/ẩn các widget tương ứng.
void Screen4View::showWinScreen()
{
    image3.setVisible(true);       // Bảng chúc mừng qua màn
    flexButton1.setVisible(false); // Ẩn nút điều khiển đang chơi
    flexButton4.setVisible(true);
    flexButton5.setVisible(true);
}
