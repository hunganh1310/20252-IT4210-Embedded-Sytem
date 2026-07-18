#include <gui/common/TetrisData.hpp>
#include <images/BitmapDatabase.hpp>

namespace tetris
{
// 7 hình gốc, canh về góc trái-trên trong khung 4x4.
const uint8_t TETROMINOS[TETRIS_SHAPES][4][4] = {
    // Hình I
    { {0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0} },
    // Hình J
    { {1,0,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0} },
    // Hình L
    { {0,0,1,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0} },
    // Hình O (vuông)
    { {1,1,0,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0} },
    // Hình S
    { {0,1,1,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0} },
    // Hình T
    { {0,1,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0} },
    // Hình Z
    { {1,1,0,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0} }
};

// 19 biến thể (đã xoay sẵn) dùng dựng địa hình chướng ngại.
const uint8_t TETROMINOS_INIT[TETRIS_SHAPES_INIT][4][4] = {
    // I - ngang / dọc
    { {0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0} },
    { {1,0,0,0}, {1,0,0,0}, {1,0,0,0}, {1,0,0,0} },
    // J - 0/90/180/270
    { {1,0,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0} },
    { {1,1,0,0}, {1,0,0,0}, {1,0,0,0}, {0,0,0,0} },
    { {1,1,1,0}, {0,0,1,0}, {0,0,0,0}, {0,0,0,0} },
    { {0,1,0,0}, {0,1,0,0}, {1,1,0,0}, {0,0,0,0} },
    // L - 0/90/180/270
    { {0,0,1,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0} },
    { {1,0,0,0}, {1,0,0,0}, {1,1,0,0}, {0,0,0,0} },
    { {1,1,1,0}, {1,0,0,0}, {0,0,0,0}, {0,0,0,0} },
    { {1,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,0,0,0} },
    // O
    { {1,1,0,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0} },
    // S - 0/90
    { {0,1,1,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0} },
    { {1,0,0,0}, {1,1,0,0}, {0,1,0,0}, {0,0,0,0} },
    // T - 0/90/180/270
    { {0,1,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0} },
    { {1,0,0,0}, {1,1,0,0}, {1,0,0,0}, {0,0,0,0} },
    { {1,1,1,0}, {0,1,0,0}, {0,0,0,0}, {0,0,0,0} },
    { {0,1,0,0}, {1,1,0,0}, {0,1,0,0}, {0,0,0,0} },
    // Z - 0/90
    { {1,1,0,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0} },
    { {0,1,0,0}, {1,1,0,0}, {1,0,0,0}, {0,0,0,0} }
};

// Biến thể -> loại hình gốc (VD: biến thể 2..5 đều là hình J nên cùng ánh xạ về 1).
const uint8_t INIT_BLOCKS_TYPE[TETRIS_SHAPES_INIT] = {
    0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 4, 4, 5, 5, 5, 5, 6, 6
};

const touchgfx::BitmapId TETROMINO_IMAGES[TETRIS_SHAPES] = {
    BITMAP_BLUE_ID,         // I
    BITMAP_RED_ID,          // J
    BITMAP_ORANGE_ID,       // L
    BITMAP_YELLOW_ID,       // O
    BITMAP_GREEN_ID,        // S
    BITMAP_PURPLE_ID,       // T
    BITMAP_BLUE_BRIGHT_ID   // Z
};

const touchgfx::BitmapId TETROMINO_NEXT_IMAGES[TETRIS_SHAPES] = {
    BITMAP_BLUE9PX_ID,         // I
    BITMAP_RED9PX_ID,          // J
    BITMAP_ORANGE9PX_ID,       // L
    BITMAP_YELLOW9PX_ID,       // O
    BITMAP_GREEN9PX_ID,        // S
    BITMAP_PURPLE9PX_ID,       // T
    BITMAP_BLUE_BRIGHT9PX_ID   // Z
};
}
