#ifndef SCREEN4VIEW_HPP
#define SCREEN4VIEW_HPP

#include <gui_generated/screen4_screen/Screen4ViewBase.hpp>
#include <gui/screen4_screen/Screen4Presenter.hpp>
#include <gui/common/TetrisView.hpp>
#include <texts/TextKeysAndLanguages.hpp>

/*
 * Screen4View - Màn chơi LEVEL 2.
 *
 * Cấu trúc giống hệt Screen3View (level 1), chỉ khác hai tham số độ khó khai báo qua hook:
 *   - INIT_BLOCKS: bố cục địa hình chướng ngại khác (gồ ghề hơn).
 *   - targetScoreValue(): 300 điểm (level 1 chỉ cần 200).
 */
class Screen4View : public TetrisView<Screen4ViewBase>
{
public:
    Screen4View();
    virtual ~Screen4View() {}

protected:
    static const int NUM_FIXED_BLOCKS = 8;
    static const tetris::InitBlock INIT_BLOCKS[NUM_FIXED_BLOCKS]; // Địa hình LEVEL 2

    virtual uint32_t initialSeed() const { return 1324; }
    virtual uint32_t targetScoreValue() const { return 300; }
    virtual const tetris::InitBlock* initBlocks(int& count) const
    {
        count = NUM_FIXED_BLOCKS;
        return INIT_BLOCKS;
    }
    virtual touchgfx::TypedText scoreText() const { return touchgfx::TypedText(T_TOADOX); }
    virtual touchgfx::TypedText gameOverScoreText() const { return touchgfx::TypedText(T_MAN3SCORE); }
    virtual void showWinScreen();

    touchgfx::Unicode::UnicodeChar textArea3Buffer[10]; // Điểm mục tiêu của màn
};

#endif
