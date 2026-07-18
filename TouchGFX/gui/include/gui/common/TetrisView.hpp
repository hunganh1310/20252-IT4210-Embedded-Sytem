#ifndef TETRIS_VIEW_HPP
#define TETRIS_VIEW_HPP

#include <cstring>
#include <touchgfx/Bitmap.hpp>
#include <touchgfx/Unicode.hpp>
#include <touchgfx/TypedText.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <gui/common/TetrisData.hpp>
#include "cmsis_os2.h"

// Hàng đợi lệnh điều khiển do Task03 (nút bấm) / Task04 (cảm biến cử chỉ PAJ7620) gửi sang.
// Mỗi phần tử là 1 ký tự: 'L' trái, 'R' phải, 'D' rơi thẳng, 'C' xoay.
extern osMessageQueueId_t Queue1Handle;

/*
 * TetrisView<Base> - toàn bộ engine Tetris dùng chung cho cả 3 màn chơi.
 *
 * Vì mỗi màn của TouchGFX phải kế thừa lớp ScreenXViewBase riêng (chứa các widget do
 * Designer sinh ra), engine được viết dưới dạng LỚP MẪU nhận Base = ScreenXViewBase.
 * Nhờ đó logic chỉ tồn tại một bản, còn phần khác nhau giữa các màn được khai báo qua
 * các hook ảo (initialSeed, targetScoreValue, initBlocks, scoreText, ...).
 *
 * Quy ước lưới: bàn chơi là lưới BOARD_HEIGHT x BOARD_WIDTH ô. Ô (0,0) nằm tại pixel
 * (ORIGIN_X, ORIGIN_Y), mỗi ô rộng BLOCK_SIZE pixel. Chuyển đổi qua lại bằng các hàm
 * boardCol/boardRow/pixelX/pixelY thay cho việc rải phép chia thủ công như bản cũ.
 *
 * Lưu ý: mọi truy cập tới thành viên của Base (widget, add/remove/invalidate) phải qua
 * this-> vì Base là lớp cơ sở phụ thuộc tham số mẫu.
 */
template <class Base>
class TetrisView : public Base
{
public:
    virtual ~TetrisView() {}

    // Được TouchGFX gọi mỗi khung hình (~60 lần/giây) - nhịp đập của game loop.
    virtual void handleTickEvent();

protected:
    static const int BLOCK_SIZE = 15;   // cạnh 1 ô (pixel), khớp bitmap khối 15px
    static const int BOARD_WIDTH = 10;
    static const int BOARD_HEIGHT = 20;
    static const int ORIGIN_X = 20;     // pixel X của ô lưới (0,0)
    static const int ORIGIN_Y = 10;     // pixel Y của ô lưới (0,0)
    static const int NEXT_BLOCK_SIZE = 9;
    static const int BAG_SIZE = tetris::TETRIS_SHAPES;

    // ----- Trạng thái ván chơi -----
    bool board[BOARD_HEIGHT][BOARD_WIDTH];                   // true = ô đã bị khối đứng yên chiếm
    touchgfx::Image blocks[4][4];                            // widget khối đang rơi
    touchgfx::Image staticBlocks[BOARD_HEIGHT][BOARD_WIDTH]; // widget các khối đã đóng băng
    touchgfx::Image nextBlocks[4][4];                        // widget xem trước khối kế tiếp
    uint8_t currentTetromino[4][4];                          // hình hiện tại (đã tính cả xoay)

    int currentX;        // pixel góc trái-trên của khung 4x4 đang rơi
    int currentY;
    int currentShape;    // loại hình đang rơi (0..6) - quyết định màu bitmap
    int nextShape;       // loại hình kế tiếp
    uint32_t tickCount;  // chia nhịp đọc input và nhịp rơi
    uint32_t score;
    uint32_t targetScore; // mốc điểm để thắng màn; 0 = chế độ endless (không có điều kiện thắng)
    bool isGameOver;
    bool isWin;

    // Bag Randomizer 7-bag: mỗi 7 khối luôn có đúng 1 hình mỗi loại.
    uint8_t bag[BAG_SIZE];
    int bagIndex;
    uint32_t randomSeed;

    touchgfx::Unicode::UnicodeChar textArea1Buffer[10]; // điểm hiển thị khi đang chơi
    touchgfx::Unicode::UnicodeChar textArea2Buffer[10]; // điểm trên bảng Game Over

    // ----- Hàm khởi động ván (gọi từ constructor của lớp con, khi vtable đã sẵn sàng) -----
    void startGame();

    // ----- Thuật toán engine -----
    void drawTetromino();
    void drawNextTetromino();
    bool checkCollision(int newX, int newY);
    void createNewTetromino();
    void freezeTetromino();
    void rotateTetromino();
    void checkAndClearLines();
    void clearScreen();
    void placeInitBlocks();
    void updateScore();
    void showGameOverScreen();

    // Bag Randomizer
    void initializeBag();
    void shuffleBag();
    int getNextShape();
    uint32_t simpleRandom();

    // ----- Đổi hệ toạ độ pixel <-> ô lưới (thay cho phép chia thủ công rải khắp nơi) -----
    static int boardCol(int px) { return (px - ORIGIN_X) / BLOCK_SIZE; }
    static int boardRow(int py) { return (py - ORIGIN_Y) / BLOCK_SIZE; }
    static int pixelX(int col)  { return ORIGIN_X + col * BLOCK_SIZE; }
    static int pixelY(int row)  { return ORIGIN_Y + row * BLOCK_SIZE; }

    // ===== Các hook mà lớp con cấu hình cho từng chế độ chơi =====
    // Seed khởi tạo cho bộ sinh số giả ngẫu nhiên.
    virtual uint32_t initialSeed() const = 0;
    // Mốc điểm thắng màn; mặc định 0 = endless (không bao giờ thắng).
    virtual uint32_t targetScoreValue() const { return 0; }
    // Danh sách khối chướng ngại đầu màn; mặc định rỗng (không có địa hình).
    virtual const tetris::InitBlock* initBlocks(int& count) const { count = 0; return 0; }
    // Kiểu chữ hiển thị điểm hiện tại / điểm trên bảng Game Over.
    virtual touchgfx::TypedText scoreText() const = 0;
    virtual touchgfx::TypedText gameOverScoreText() const = 0;
    // Bảng "thắng màn" chỉ có ở các level (Screen3/4); chế độ endless để trống.
    virtual void showWinScreen() {}
};

// ================================ CÀI ĐẶT ================================

template <class Base>
void TetrisView<Base>::startGame()
{
    currentX = pixelX(4);   // cột 4: giữa đỉnh bàn
    currentY = ORIGIN_Y;    // hàng 0
    currentShape = 0;
    tickCount = 0;
    score = 0;
    isGameOver = false;
    isWin = false;
    randomSeed = initialSeed();
    targetScore = targetScoreValue();

    initializeBag();
    nextShape = getNextShape();

    memset(board, 0, sizeof(board));
    memcpy(currentTetromino, tetris::TETROMINOS[currentShape], sizeof(currentTetromino));

    drawTetromino();
    drawNextTetromino();
    placeInitBlocks();
    updateScore();
}

// Vẽ khối đang rơi: gỡ 16 widget cũ rồi add lại những ô có giá trị 1, nên dùng chung
// được cho cả di chuyển lẫn xoay (hình đổi thì số ô cũng đổi).
template <class Base>
void TetrisView<Base>::drawTetromino()
{
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            this->remove(blocks[y][x]);

    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            if (currentTetromino[y][x])
            {
                blocks[y][x].setXY(currentX + x * BLOCK_SIZE, currentY + y * BLOCK_SIZE);
                blocks[y][x].setBitmap(touchgfx::Bitmap(tetris::TETROMINO_IMAGES[currentShape]));
                this->add(blocks[y][x]);
            }
}

// Vẽ khối kế tiếp trong khung xem trước ở góc phải, dùng bitmap 9px.
template <class Base>
void TetrisView<Base>::drawNextTetromino()
{
    // Mỗi hình chiếm phần khác nhau trong khung 4x4 nên phải bù trừ thủ công cho cân giữa.
    int startX, startY;
    switch (nextShape)
    {
    case 0: // Hình I
        startX = 187; startY = 127; break;
    case 3: // Hình O
        startX = 195; startY = 123; break;
    default: // J, L, S, T, Z
        startX = 192; startY = 123; break;
    }

    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            this->remove(nextBlocks[y][x]);

    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            if (tetris::TETROMINOS[nextShape][y][x])
            {
                nextBlocks[y][x].setXY(startX + x * NEXT_BLOCK_SIZE, startY + y * NEXT_BLOCK_SIZE);
                nextBlocks[y][x].setBitmap(touchgfx::Bitmap(tetris::TETROMINO_NEXT_IMAGES[nextShape]));
                this->add(nextBlocks[y][x]);
            }
}

// true nếu đặt khối hiện tại ở pixel (newX,newY) sẽ va chạm: chạm đáy, vượt biên
// trái/phải, hoặc đè lên khối đã đóng băng.
template <class Base>
bool TetrisView<Base>::checkCollision(int newX, int newY)
{
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            if (currentTetromino[y][x])
            {
                int col = boardCol(newX) + x;
                int row = boardRow(newY) + y;
                if (row >= BOARD_HEIGHT) return true;           // chạm đáy
                if (col < 0 || col >= BOARD_WIDTH) return true;  // chạm biên
                if (board[row][col]) return true;               // chạm khối khác
            }
    return false;
}

// Quét bàn tìm hàng đầy, cộng điểm, xoá hàng, dồn khối xuống và kiểm tra thắng màn.
template <class Base>
void TetrisView<Base>::checkAndClearLines()
{
    int fullLines[BOARD_HEIGHT] = {0};
    int numFullLines = 0;

    // Duyệt từ dưới lên nên fullLines[0] là hàng đầy thấp nhất - mốc để dồn khối phía trên.
    for (int y = BOARD_HEIGHT - 1; y >= 0; y--)
    {
        bool isLineFull = true;
        for (int x = 0; x < BOARD_WIDTH; x++)
            if (!board[y][x]) { isLineFull = false; break; }
        if (isLineFull) fullLines[numFullLines++] = y;
    }

    if (numFullLines > 0)
    {
        // Thưởng tăng phi tuyến để khuyến khích ăn nhiều hàng cùng lúc.
        if (numFullLines == 1)      score += 10;
        else if (numFullLines == 2) score += 30;
        else if (numFullLines == 3) score += 50;
        else                        score += 100; // 4 hàng
        updateScore();

        // Xoá các hàng đầy: gỡ widget và giải phóng ô trong board.
        for (int i = 0; i < numFullLines; i++)
        {
            int lineY = fullLines[i];
            for (int x = 0; x < BOARD_WIDTH; x++)
            {
                this->remove(staticBlocks[lineY][x]);
                board[lineY][x] = false;
            }
        }

        // Dồn các hàng còn lại xuống. Quét ngược từ hàng đầy thấp nhất lên đỉnh để ô đích
        // luôn đã xử lý xong, tránh ghi đè khối chưa kịp di chuyển.
        for (int y = fullLines[0]; y >= 0; y--)
        {
            int shiftDown = 0;
            for (int i = 0; i < numFullLines; i++)
                if (fullLines[i] > y) shiftDown++;

            if (shiftDown > 0)
                for (int x = 0; x < BOARD_WIDTH; x++)
                    if (board[y][x])
                    {
                        board[y + shiftDown][x] = true;
                        board[y][x] = false;

                        // Copy cả widget để giữ nguyên màu bitmap khi dồn xuống.
                        staticBlocks[y + shiftDown][x] = staticBlocks[y][x];
                        staticBlocks[y + shiftDown][x].setXY(pixelX(x), pixelY(y + shiftDown));
                        this->add(staticBlocks[y + shiftDown][x]);
                        this->remove(staticBlocks[y][x]);
                    }
        }

        this->invalidate();
    }

    // Điều kiện thắng: kiểm tra ngoài khối if trên vì điểm chỉ tăng khi ăn hàng.
    // targetScore == 0 (endless) thì không bao giờ vào nhánh này.
    if (targetScore != 0 && score >= targetScore)
    {
        isWin = true;
        clearScreen();
        showWinScreen();
        this->invalidate();
    }
}

// Đóng băng khối đang rơi vào board rồi kiểm tra ăn hàng / thắng màn.
template <class Base>
void TetrisView<Base>::freezeTetromino()
{
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            if (currentTetromino[y][x])
            {
                int col = boardCol(currentX) + x;
                int row = boardRow(currentY) + y;
                board[row][col] = true;
                staticBlocks[row][col].setXY(pixelX(col), pixelY(row));
                staticBlocks[row][col].setBitmap(touchgfx::Bitmap(tetris::TETROMINO_IMAGES[currentShape]));
                this->add(staticBlocks[row][col]);
            }
    checkAndClearLines();
}

// Game loop chính, chạy trên GUI task theo nhịp khung hình của TouchGFX.
// Hai công việc chia nhịp bằng tickCount:
//   - mỗi 25 tick (lệch pha 12): đọc 1 lệnh điều khiển từ Queue1
//   - mỗi 50 tick: hạ khối xuống 1 ô (trọng lực)
// Hai mốc lệch pha nhau để lệnh người chơi và bước rơi không rơi vào cùng khung hình.
// Dừng hẳn khi thua hoặc thắng để bảng kết quả không bị khối vẽ đè lên.
template <class Base>
void TetrisView<Base>::handleTickEvent()
{
    if (isGameOver || isWin) return;

    tickCount++;

    // ----- Nhịp đọc input -----
    if (tickCount % 25 == 12)
    {
        uint8_t res = 0;
        // Chỉ chờ khi chắc chắn hàng đợi có phần tử, nếu không GUI task sẽ bị treo.
        if (osMessageQueueGetCount(Queue1Handle) > 0)
        {
            osMessageQueueGet(Queue1Handle, &res, NULL, osWaitForever);
            if (res == 'L')
            {
                if (!checkCollision(currentX - BLOCK_SIZE, currentY))
                {
                    currentX -= BLOCK_SIZE;
                    drawTetromino();
                }
            }
            else if (res == 'R')
            {
                if (!checkCollision(currentX + BLOCK_SIZE, currentY))
                {
                    currentX += BLOCK_SIZE;
                    drawTetromino();
                }
            }
            else if (res == 'D')
            {
                // Hard drop: rơi thẳng tới đáy rồi chốt luôn, không cho chỉnh thêm.
                while (!checkCollision(currentX, currentY + BLOCK_SIZE))
                {
                    currentY += BLOCK_SIZE;
                    drawTetromino();
                }
                freezeTetromino();
                createNewTetromino();
            }
            else if (res == 'C')
            {
                rotateTetromino();
            }
        }
    }

    // ----- Nhịp rơi tự do -----
    if (tickCount % 50 == 0)
    {
        if (checkCollision(currentX, currentY + BLOCK_SIZE))
        {
            freezeTetromino();
            createNewTetromino();
        }
        else
        {
            currentY += BLOCK_SIZE;
            drawTetromino();
        }
    }

    this->invalidate();
}

// Đưa khối kế tiếp ra sân, rút khối mới từ túi. Nếu vị trí sinh ở đỉnh đã bị chiếm -> Game Over.
template <class Base>
void TetrisView<Base>::createNewTetromino()
{
    if (isGameOver || isWin) return;

    currentX = pixelX(4);
    currentY = ORIGIN_Y;
    currentShape = nextShape;
    nextShape = getNextShape();

    memcpy(currentTetromino, tetris::TETROMINOS[currentShape], sizeof(currentTetromino));
    drawNextTetromino();

    // Vị trí sinh đã bị chiếm nghĩa là hết chỗ chơi.
    if (checkCollision(currentX, currentY))
    {
        isGameOver = true;
        showGameOverScreen();
    }
    else
    {
        drawTetromino();
    }
}

// Xoay khối 90 độ theo chiều kim đồng hồ, chỉ áp dụng nếu không va chạm.
// Không có cơ chế "wall kick" nên khối sát tường sẽ đơn giản là không xoay được.
template <class Base>
void TetrisView<Base>::rotateTetromino()
{
    if (currentShape == 3) return; // Hình O không cần xoay

    uint8_t temp[4][4];
    uint8_t rotated[4][4] = {0};
    memcpy(temp, currentTetromino, sizeof(temp));

    // Hình I trải hết khung 4x4 (tâm 2,2); các hình khác chỉ chiếm 3x3 góc trái-trên (tâm 1,1).
    int centerX = (currentShape == 0) ? 2 : 1;
    int centerY = (currentShape == 0) ? 2 : 1;

    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
        {
            if (currentShape == 0)
            {
                // Xoay cả khung 4x4 (transpose + lật) vì hình I trải hết chiều rộng khung.
                rotated[x][3 - y] = temp[y][x];
            }
            else
            {
                // Công thức xoay 90 độ quanh tâm: (x,y) -> (-y,x).
                int newX = centerX - (y - centerY);
                int newY = centerY + (x - centerX);
                if (newX >= 0 && newX < 4 && newY >= 0 && newY < 4)
                    rotated[newY][newX] = temp[y][x];
            }
        }

    // Chỉ xoay nếu hình mới không va chạm.
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            if (rotated[y][x])
            {
                int col = boardCol(currentX) + x;
                int row = boardRow(currentY) + y;
                if (col < 0 || col >= BOARD_WIDTH || row < 0 || row >= BOARD_HEIGHT || board[row][col])
                    return;
            }

    memcpy(currentTetromino, rotated, sizeof(currentTetromino));
    drawTetromino();
    this->invalidate();
}

// Dựng địa hình đầu màn từ danh sách khối chướng ngại (lớp con cung cấp qua initBlocks()).
// Sau bước này chúng không khác gì khối do người chơi đóng băng, vẫn bị ăn hàng bình thường.
template <class Base>
void TetrisView<Base>::placeInitBlocks()
{
    int count = 0;
    const tetris::InitBlock* fixed = initBlocks(count);
    for (int i = 0; i < count; i++)
    {
        int col0 = boardCol(fixed[i].pixelX);
        int row0 = boardRow(fixed[i].pixelY);
        int variant = fixed[i].shapeVariant;
        for (int y = 0; y < 4; y++)
            for (int x = 0; x < 4; x++)
                if (tetris::TETROMINOS_INIT[variant][y][x])
                {
                    int col = col0 + x;
                    int row = row0 + y;
                    board[row][col] = true;
                    staticBlocks[row][col].setXY(pixelX(col), pixelY(row));
                    staticBlocks[row][col].setBitmap(
                        touchgfx::Bitmap(tetris::TETROMINO_IMAGES[tetris::INIT_BLOCKS_TYPE[variant]]));
                    this->add(staticBlocks[row][col]);
                }
    }
}

// Cập nhật ô hiển thị điểm hiện tại (kiểu chữ do lớp con quyết định qua scoreText()).
template <class Base>
void TetrisView<Base>::updateScore()
{
    touchgfx::Unicode::snprintf(textArea1Buffer, sizeof(textArea1Buffer), "%d", score);
    this->textArea1.setWildcard(textArea1Buffer);
    this->textArea1.setTypedText(scoreText());
    this->textArea1.invalidate();
}

// Game Over: dọn sân, hiện bảng kết quả cùng nút chơi lại / về menu, ẩn nút đang chơi.
// Bố cục bảng Game Over (image2, flexButton1/2/3, textArea2) giống nhau ở cả 3 màn.
template <class Base>
void TetrisView<Base>::showGameOverScreen()
{
    clearScreen();
    this->image2.setVisible(true);
    this->flexButton1.setVisible(false);
    this->flexButton2.setVisible(true);
    this->flexButton3.setVisible(true);

    touchgfx::Unicode::snprintf(textArea2Buffer, sizeof(textArea2Buffer), "%d", score);
    this->textArea2.setVisible(true);
    this->textArea2.setWildcard(textArea2Buffer);
    this->textArea2.setTypedText(gameOverScoreText());
    this->textArea2.invalidate();

    this->invalidate();
}

// Gỡ toàn bộ widget khối (đang rơi, đã đóng băng, xem trước) để lộ bảng kết quả phía dưới.
template <class Base>
void TetrisView<Base>::clearScreen()
{
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            this->remove(blocks[y][x]);

    for (int y = 0; y < BOARD_HEIGHT; y++)
        for (int x = 0; x < BOARD_WIDTH; x++)
            if (board[y][x])
            {
                this->remove(staticBlocks[y][x]);
                board[y][x] = false;
            }

    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            this->remove(nextBlocks[y][x]);
}

// ========================= BAG RANDOMIZER =========================
// Cơ chế 7-bag chuẩn của Tetris hiện đại: xáo trộn đủ 7 hình rồi phát lần lượt, hết túi
// mới xáo lại. Nhờ vậy mỗi 7 khối luôn có đúng 1 hình mỗi loại, tránh random tự do phát
// trùng liên tục hoặc "đói" hình I quá lâu.

template <class Base>
void TetrisView<Base>::initializeBag()
{
    for (int i = 0; i < BAG_SIZE; i++)
        bag[i] = i;
    shuffleBag();
    bagIndex = 0;
}

template <class Base>
void TetrisView<Base>::shuffleBag()
{
    // Fisher-Yates shuffle.
    for (int i = BAG_SIZE - 1; i > 0; i--)
    {
        int j = simpleRandom() % (i + 1);
        uint8_t t = bag[i];
        bag[i] = bag[j];
        bag[j] = t;
    }
}

template <class Base>
int TetrisView<Base>::getNextShape()
{
    if (bagIndex >= BAG_SIZE)
    {
        shuffleBag();
        bagIndex = 0;
    }
    return bag[bagIndex++];
}

template <class Base>
uint32_t TetrisView<Base>::simpleRandom()
{
    // Linear Congruential Generator: next = a*seed + c (mod 2^32 tự động do tràn uint32_t).
    // Cộng thêm score để chuỗi random phụ thuộc cách chơi, nhờ đó mỗi ván không lặp lại
    // y hệt dù seed cố định (board không có RTC/nguồn entropy).
    randomSeed = randomSeed * 1664525 + 1013904223 + score - 2611;
    return randomSeed;
}

#endif
