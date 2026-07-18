#include <gui/screen1_screen/Screen1View.hpp>

// Chế độ endless không có địa hình đầu màn cũng không có mốc thắng, nên constructor
// chỉ cần khởi động ván. Mọi logic game do TetrisView xử lý.
Screen1View::Screen1View()
{
    startGame();
}
