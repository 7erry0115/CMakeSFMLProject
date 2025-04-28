#include "GameScene.hpp"
#include "../winscene/WinScene.hpp"
#include <sstream>
#include <string>

// 建構式
GameScene::GameScene(const sf::Font &font, const sf::Vector2u &windowSize)
    : font(font),
      remainingTimeBlack(300.f),
      remainingTimeWhite(300.f),
      currentPlayer(Player::BLACK),
      previewRow(-1),
      previewCol(-1),
      isActive(true) {
    background.setSize(sf::Vector2f(windowSize));
    background.setFillColor(sf::Color(220, 220, 220));

    // 計算棋盤區域 (棋盤顯示於畫面左側)
    cellSize = (windowSize.x / 2.f) / boardSize;
    cells.resize(boardSize, std::vector<sf::RectangleShape>(boardSize));
    board.resize(boardSize, std::vector<Player>(boardSize, Player::NONE));

    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            cells[i][j].setSize(sf::Vector2f(cellSize - 2, cellSize - 2));
            cells[i][j].setFillColor(sf::Color::Green);
            cells[i][j].setPosition(sf::Vector2f(j * cellSize + 10, i * cellSize + 10));
        }
    }

    // 設置標準奧賽羅起始局面：中心四格
    if (boardSize >= 4) {
        int mid = boardSize / 2;
        board[mid - 1][mid - 1] = Player::WHITE;
        board[mid - 1][mid] = Player::BLACK;
        board[mid][mid - 1] = Player::BLACK;
        board[mid][mid] = Player::WHITE;
    }

    // 初始化雙玩家倒數文字 (分別顯示在右側)
    textBlackTimer.setFont(font);
    textBlackTimer.setCharacterSize(20);
    textBlackTimer.setFillColor(sf::Color::Black);
    textBlackTimer.setPosition(sf::Vector2f(windowSize.x / 2.f + 20, 50));

    textWhiteTimer.setFont(font);
    textWhiteTimer.setCharacterSize(20);
    textWhiteTimer.setFillColor(sf::Color::Black);
    textWhiteTimer.setPosition(sf::Vector2f(windowSize.x / 2.f + 20, 100));
}

// 取得在 (row, col) 落子後可以翻轉的對方棋子列表
std::vector<std::pair<int, int> > GameScene::getFlippableStones(int row, int col, Player player) const {
    std::vector<std::pair<int, int> > totalFlips;
    // 若該格不為空，直接回傳空列表
    if (board[row][col] != Player::NONE)
        return totalFlips;

    // 定義 8 個方向
    std::vector<std::pair<int, int> > directions = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1}, {0, 1},
        {1, -1}, {1, 0}, {1, 1}
    };

    for (auto &dir: directions) {
        int dx = dir.first, dy = dir.second;
        int x = row + dx, y = col + dy;
        std::vector<std::pair<int, int> > flipList;

        // 檢查第一格: 必須在邊界內且為對方棋子
        if (x < 0 || x >= boardSize || y < 0 || y >= boardSize)
            continue;
        if (board[x][y] == Player::NONE || board[x][y] == player)
            continue;

        // 往該方向延伸，累積對方棋子
        while (x >= 0 && x < boardSize && y >= 0 && y < boardSize) {
            if (board[x][y] == Player::NONE) {
                flipList.clear();
                break;
            }
            if (board[x][y] == player) {
                break;
            }
            flipList.push_back({x, y});
            x += dx;
            y += dy;
        }
        // 若延伸時最後遇到己方棋子，且至少有一顆對方棋子，則該方向合法
        if (x >= 0 && x < boardSize && y >= 0 && y < boardSize && board[x][y] == player && !flipList.empty()) {
            totalFlips.insert(totalFlips.end(), flipList.begin(), flipList.end());
        }
    }
    return totalFlips;
}

// 判斷指定玩家是否有至少一步合法棋步
bool GameScene::hasLegalMove(Player player) const {
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            if (board[i][j] == Player::NONE) {
                auto flips = getFlippableStones(i, j, player);
                if (!flips.empty()) {
                    return true;
                }
            }
        }
    }
    return false;
}

void GameScene::handleEvent(const sf::Event &event) {
    // 若場景已標記為無效則不處理事件
    if (!isActive) return;

    // 處理滑鼠移動事件：預覽可下位置及預覽可翻轉棋子
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        if (mousePos.x >= 10 && mousePos.x <= (10 + boardSize * cellSize) &&
            mousePos.y >= 10 && mousePos.y <= (10 + boardSize * cellSize)) {
            int j = static_cast<int>((mousePos.x - 10) / cellSize);
            int i = static_cast<int>((mousePos.y - 10) / cellSize);
            if (board[i][j] == Player::NONE) {
                auto flips = getFlippableStones(i, j, currentPlayer);
                if (!flips.empty()) {
                    previewRow = i;
                    previewCol = j;
                    previewFlips = flips;
                } else {
                    previewRow = -1;
                    previewCol = -1;
                    previewFlips.clear();
                }
            } else {
                previewRow = -1;
                previewCol = -1;
                previewFlips.clear();
            }
        } else {
            previewRow = -1;
            previewCol = -1;
            previewFlips.clear();
        }
    }

    // 處理滑鼠點擊事件：下棋
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
            // 限制點擊範圍在棋盤內 (左側區域，自 x = 10 開始)
            if (mousePos.x >= 10 && mousePos.x <= (10 + boardSize * cellSize) &&
                mousePos.y >= 10 && mousePos.y <= (10 + boardSize * cellSize)) {
                int j = static_cast<int>((mousePos.x - 10) / cellSize);
                int i = static_cast<int>((mousePos.y - 10) / cellSize);
                // 僅在空格且至少能翻轉一顆對方棋子時，下棋有效
                if (board[i][j] == Player::NONE) {
                    auto flips = getFlippableStones(i, j, currentPlayer);
                    if (!flips.empty()) {
                        // 合法步：放置棋子與翻轉所有對應對方棋子
                        board[i][j] = currentPlayer;
                        for (auto &pos: flips) {
                            board[pos.first][pos.second] = currentPlayer;
                        }
                        switchPlayer();
                        // 下棋後清除預覽資料
                        previewRow = -1;
                        previewCol = -1;
                        previewFlips.clear();
                    }
                }
            }
        }
    }

    // 其他鍵盤事件（例如切換場景）保持現有邏輯
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            // 此處可作相關處理
        }
    }
}

std::shared_ptr<Scene> GameScene::update(sf::Time delta) {
    // 若場景已失效則不更新
    if (!isActive) return nullptr;

    // 僅減少目前回合玩家的倒數時間並檢查是否時間到
    if (currentPlayer == Player::BLACK) {
        remainingTimeBlack -= delta.asSeconds();
        if (remainingTimeBlack <= 0) {
            remainingTimeBlack = 0;
            isActive = false; // 標記此場景不可再處理事件與更新
            return std::make_shared<WinScene>(font,
                                              sf::Vector2u(static_cast<unsigned int>(background.getSize().x),
                                                           static_cast<unsigned int>(background.getSize().y)),
                                              "Time's up. White Wins!");
        }
    } else {
        remainingTimeWhite -= delta.asSeconds();
        if (remainingTimeWhite <= 0) {
            remainingTimeWhite = 0;
            isActive = false;
            return std::make_shared<WinScene>(font,
                                              sf::Vector2u(static_cast<unsigned int>(background.getSize().x),
                                                           static_cast<unsigned int>(background.getSize().y)),
                                              "Time's up. Black Wins!");
        }
    }

    // 更新倒數文字顯示
    textBlackTimer.setString("Black Time Countdown: " + std::to_string(static_cast<int>(remainingTimeBlack)) + u8"s");
    textWhiteTimer.setString("White Time Countdown: " + std::to_string(static_cast<int>(remainingTimeWhite)) + u8"s");

    // 檢查當前玩家是否有合法步：若無，必須跳步
    if (!hasLegalMove(currentPlayer)) {
        // 若對方也無合法步，則遊戲結束；否則跳步
        Player opponent = (currentPlayer == Player::BLACK) ? Player::WHITE : Player::BLACK;
        if (!hasLegalMove(opponent)) {
            // 雙方皆無合法步，遊戲結束，由多者獲勝
            int countBlack = 0, countWhite = 0;
            for (int i = 0; i < boardSize; ++i) {
                for (int j = 0; j < boardSize; ++j) {
                    if (board[i][j] == Player::BLACK)
                        countBlack++;
                    else if (board[i][j] == Player::WHITE)
                        countWhite++;
                }
            }
            std::string winner;
            if (countBlack > countWhite)
                winner = "Black Wins!";
            else if (countWhite > countBlack)
                winner = "White Wins!";
            else
                winner = "Even!";
            isActive = false;
            return std::make_shared<WinScene>(font,
                                              sf::Vector2u(static_cast<unsigned int>(background.getSize().x),
                                                           static_cast<unsigned int>(background.getSize().y)),
                                              winner);
        } else {
            // 當前玩家無合法步，則必須跳步，由對方下棋
            switchPlayer();
        }
    }

    return nullptr;
}

void GameScene::draw(sf::RenderWindow &window) {
    window.draw(background);
    // 畫出棋盤格
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            window.draw(cells[i][j]);
        }
    }

    // 畫出棋子 (以圓形代表)
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            if (board[i][j] != Player::NONE) {
                sf::CircleShape piece(cellSize / 2.f - 4);
                piece.setOrigin(piece.getRadius(), piece.getRadius());
                sf::Vector2f pos = cells[i][j].getPosition();
                pos.x += (cellSize - 2) / 2.f;
                pos.y += (cellSize - 2) / 2.f;
                piece.setPosition(pos);
                if (board[i][j] == Player::BLACK)
                    piece.setFillColor(sf::Color::Black);
                else if (board[i][j] == Player::WHITE)
                    piece.setFillColor(sf::Color::White);
                window.draw(piece);
            }
        }
    }

    // 畫出倒數計時文字
    window.draw(textBlackTimer);
    window.draw(textWhiteTimer);

    // 畫出預覽效果：在可下且合法的格子上以半透明形式顯示當前玩家棋子，
    // 以及在預覽翻轉位置上以半透明呈現翻轉效果
    if (previewRow != -1 && previewCol != -1) {
        // 預覽落子
        sf::CircleShape previewPiece(cellSize / 2.f - 4);
        previewPiece.setOrigin(previewPiece.getRadius(), previewPiece.getRadius());
        sf::Vector2f pos = cells[previewRow][previewCol].getPosition();
        pos.x += (cellSize - 2) / 2.f;
        pos.y += (cellSize - 2) / 2.f;
        previewPiece.setPosition(pos);
        if (currentPlayer == Player::BLACK)
            previewPiece.setFillColor(sf::Color(0, 0, 0, 128));
        else if (currentPlayer == Player::WHITE)
            previewPiece.setFillColor(sf::Color(255, 255, 255, 128));
        window.draw(previewPiece);

        // 預覽翻轉效果：在可翻轉的位置上也以半透明呈現當前玩家顏色
        for (const auto &flipPos: previewFlips) {
            int i = flipPos.first;
            int j = flipPos.second;
            sf::CircleShape flipPreview(cellSize / 2.f - 4);
            flipPreview.setOrigin(flipPreview.getRadius(), flipPreview.getRadius());
            sf::Vector2f cellPos = cells[i][j].getPosition();
            cellPos.x += (cellSize - 2) / 2.f;
            cellPos.y += (cellSize - 2) / 2.f;
            flipPreview.setPosition(cellPos);
            if (currentPlayer == Player::BLACK)
                flipPreview.setFillColor(sf::Color(0, 0, 0, 128));
            else if (currentPlayer == Player::WHITE)
                flipPreview.setFillColor(sf::Color(255, 255, 255, 128));
            window.draw(flipPreview);
        }
    }
}

void GameScene::switchPlayer() {
    // 切換玩家：不重置倒數時間，並清除預覽資料
    currentPlayer = (currentPlayer == Player::BLACK) ? Player::WHITE : Player::BLACK;
    previewRow = -1;
    previewCol = -1;
    previewFlips.clear();
}
