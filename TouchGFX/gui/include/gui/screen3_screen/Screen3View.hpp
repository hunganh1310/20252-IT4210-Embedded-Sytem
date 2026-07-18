#ifndef SCREEN3VIEW_HPP
#define SCREEN3VIEW_HPP

#include <gui_generated/screen3_screen/Screen3ViewBase.hpp>
#include <gui/screen3_screen/Screen3Presenter.hpp>
#include <gui/common/TetrisView.hpp>
#include <texts/TextKeysAndLanguages.hpp>

/*
 * Screen3View - Màn chơi LEVEL 1.
 *
 * Dùng chung engine TetrisView với Screen1 (endless), chỉ khác ở phần cấu hình khai báo
 * qua các hook:
 *   - Bàn bắt đầu với NUM_FIXED_BLOCKS khối chướng ngại (INIT_BLOCKS).
 *   - Có điều kiện thắng: đạt targetScoreValue() điểm thì hiện bảng qua màn.
 */
class Screen3View : public TetrisView<Screen3ViewBase>
{
public:
    Screen3View();
    virtual ~Screen3View() {}

protected:
    static const int NUM_FIXED_BLOCKS = 8;
    static const tetris::InitBlock INIT_BLOCKS[NUM_FIXED_BLOCKS]; // Địa hình LEVEL 1

    virtual uint32_t initialSeed() const { return 1324; }
    virtual uint32_t targetScoreValue() const { return 200; }
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
