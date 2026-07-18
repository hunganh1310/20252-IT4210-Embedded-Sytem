#ifndef TETRIS_DATA_HPP
#define TETRIS_DATA_HPP

#include <stdint.h>
#include <touchgfx/Bitmap.hpp>

/*
 * Dữ liệu tĩnh dùng chung cho cả 3 màn Tetris (Screen1 endless, Screen3/4 các level).
 * Tách riêng và định nghĩa MỘT lần trong TetrisData.cpp để không nhân bản các bảng
 * ra flash 3 lần như bản cũ (mỗi màn tự khai báo một bản sao y hệt).
 */
namespace tetris
{
// Số loại tetromino gốc (I, J, L, O, S, T, Z) và số biến thể đã xoay sẵn (dựng địa hình).
const int TETRIS_SHAPES = 7;
const int TETRIS_SHAPES_INIT = 19;

// Ma trận 4x4 của 7 hình gốc. Thứ tự phải khớp TETROMINO_IMAGES/TETROMINO_NEXT_IMAGES.
extern const uint8_t TETROMINOS[TETRIS_SHAPES][4][4];

// "Catalogue" 19 biến thể ở mọi góc xoay - chỉ dùng để đặt khối chướng ngại đầu màn,
// nhờ vậy dựng địa hình chỉ là tra bảng, không cần chạy thuật toán xoay.
extern const uint8_t TETROMINOS_INIT[TETRIS_SHAPES_INIT][4][4];

// Ánh xạ biến thể trong TETROMINOS_INIT về loại hình gốc (để lấy đúng màu bitmap).
extern const uint8_t INIT_BLOCKS_TYPE[TETRIS_SHAPES_INIT];

// Bitmap khối: 15px vẽ trên bàn chơi, 9px vẽ trong khung xem trước. Tra theo loại hình gốc.
extern const touchgfx::BitmapId TETROMINO_IMAGES[TETRIS_SHAPES];
extern const touchgfx::BitmapId TETROMINO_NEXT_IMAGES[TETRIS_SHAPES];

// Một khối chướng ngại của địa hình ban đầu (mỗi level có bố cục riêng, xem ScreenXView.cpp).
struct InitBlock
{
    int pixelX;       // toạ độ pixel góc trái-trên (phải nằm đúng lưới: 20 + 15k)
    int pixelY;       // 10 + 15k
    int shapeVariant; // chỉ số trong TETROMINOS_INIT
};
}

#endif
