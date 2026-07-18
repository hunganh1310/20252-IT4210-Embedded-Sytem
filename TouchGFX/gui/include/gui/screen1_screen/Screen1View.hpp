#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include <gui/common/TetrisView.hpp>
#include <texts/TextKeysAndLanguages.hpp>

/*
 * Screen1View - Màn chơi chế độ ENDLESS (chơi không giới hạn điểm).
 *
 * Toàn bộ engine Tetris nằm ở lớp mẫu TetrisView (xem TetrisView.hpp để hiểu quy ước
 * lưới/toạ độ). Màn này chỉ cấu hình phần riêng của chế độ endless:
 *   - seed random riêng,
 *   - không có địa hình chướng ngại (dùng initBlocks() mặc định rỗng),
 *   - không có mốc thắng (targetScoreValue() mặc định 0), nên chơi đến khi thua.
 */
class Screen1View : public TetrisView<Screen1ViewBase>
{
public:
    Screen1View();
    virtual ~Screen1View() {}

protected:
    virtual uint32_t initialSeed() const { return 2611; }
    virtual touchgfx::TypedText scoreText() const { return touchgfx::TypedText(T_MAN1); }
    virtual touchgfx::TypedText gameOverScoreText() const { return touchgfx::TypedText(T_MAN1SCORE); }
};

#endif
