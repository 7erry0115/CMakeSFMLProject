#include "GameScene.hpp"
#include "../winscene/WinScene.hpp"
#include "../startscene/StartScene.hpp"  // 用於返回起始畫面
#include <sstream>
#include <string>
#include <random>
#include <iostream>

// 建構式：加入第三個參數 useAIOpponent 並初始化 Home 按鈕
GameScene::GameScene(const sf::Font& font, const sf::Vector2u& windowSize, bool useAIOpponent)
    : font(font),
    remainingTimeBlack(300.f),
    remainingTimeWhite(300.f),
    currentPlayer(Player::BLACK),
    previewRow(-1),
    previewCol(-1),
    isActive(true),
    showPass(false),
    passActive(false),
    lastMoveRow(-1),
    lastMoveCol(-1),
    useAIOpponent(useAIOpponent),
    returnHome(false)
{
    background.setSize(sf::Vector2f(windowSize));
    background.setFillColor(sf::Color(173, 176, 168));

    // 設定棋盤尺寸 (棋盤顯示於左半邊)
    cellSize = (windowSize.x / 2.f) / boardSize;
    cells.resize(boardSize, std::vector<sf::RectangleShape>(boardSize));
    board.resize(boardSize, std::vector<Player>(boardSize, Player::NONE));

    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            cells[i][j].setSize(sf::Vector2f(cellSize - 2, cellSize - 2));
            cells[i][j].setFillColor(sf::Color(220, 184, 135));
            cells[i][j].setPosition(sf::Vector2f(j * cellSize + 10, i * cellSize + 10));
        }
    }

    // 設定標準 Othello 棋局中心四子
    if (boardSize >= 4) {
        int mid = boardSize / 2;
        board[mid - 1][mid - 1] = Player::WHITE;
        board[mid - 1][mid] = Player::BLACK;
        board[mid][mid - 1] = Player::BLACK;
        board[mid][mid] = Player::WHITE;
    }

    // 初始化倒數計時文字 (顯示於右側)
    textBlackTimer.setFont(font);
    textBlackTimer.setCharacterSize(20);
    textBlackTimer.setFillColor(sf::Color::Black);
    textBlackTimer.setPosition(sf::Vector2f(windowSize.x / 2.f + 20, 50));

    textWhiteTimer.setFont(font);
    textWhiteTimer.setCharacterSize(20);
    textWhiteTimer.setFillColor(sf::Color::Black);
    textWhiteTimer.setPosition(sf::Vector2f(windowSize.x / 2.f + 20, 100));

    // 初始化 PASS 文字
    passText.setFont(font);
    passText.setCharacterSize(24);
    passText.setFillColor(sf::Color::Red);
    passText.setString("PASS");
    passText.setPosition(sf::Vector2f(windowSize.x / 2.f + 20, 140));

    // 建立 Home 按鈕，設定大小為 100x100 並放置在右下角
    homeButton = Button(font, "Home", 30);
    homeButton.setSize(sf::Vector2f(100.f, 70.f));
    homeButton.setPosition(sf::Vector2f(windowSize.x - 110.f, windowSize.y - 80.f));
    // 調整 home 按鈕的背景與文字顏色
    homeButton.setBackgroundColor(sf::Color(84, 84, 84));
    homeButton.setTextColor(sf::Color::White);
}

std::vector<std::pair<int, int>> GameScene::getFlippableStones(int row, int col, Player player) const {
    std::vector<std::pair<int, int>> totalFlips;
    if (row < 0 || row >= boardSize || col < 0 || col >= boardSize)
        return totalFlips;
    if (board[row][col] != Player::NONE)
        return totalFlips;

    // 八個方向
    std::vector<std::pair<int, int>> directions = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1},  {1, 0},  {1, 1}
    };

    for (auto& dir : directions) {
        int dx = dir.first, dy = dir.second;
        int x = row + dx, y = col + dy;
        std::vector<std::pair<int, int>> flipList;

        if (x < 0 || x >= boardSize || y < 0 || y >= boardSize)
            continue;
        if (board[x][y] == Player::NONE || board[x][y] == player)
            continue;

        while (x >= 0 && x < boardSize && y >= 0 && y < boardSize) {
            if (board[x][y] == Player::NONE) {
                flipList.clear();
                break;
            }
            if (board[x][y] == player)
                break;
            flipList.push_back({ x, y });
            x += dx;
            y += dy;
        }
        if (x >= 0 && x < boardSize && y >= 0 && y < boardSize &&
            board[x][y] == player && !flipList.empty()) {
            totalFlips.insert(totalFlips.end(), flipList.begin(), flipList.end());
        }
    }
    return totalFlips;
}

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

void GameScene::handleEvent(const sf::Event& event) {
    // 檢查是否按下 Home 按鈕
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x),
            static_cast<float>(event.mouseButton.y));
        if (homeButton.contains(mousePos)) {
            returnHome = true;
        }
    }

    if (!isActive)
        return;

    // 處理滑鼠移動更新預覽棋步
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x),
            static_cast<float>(event.mouseMove.y));
        if (mousePos.x >= 10 && mousePos.x <= (10 + boardSize * cellSize) &&
            mousePos.y >= 10 && mousePos.y <= (10 + boardSize * cellSize)) {
            int j = static_cast<int>((mousePos.x - 10) / cellSize);
            int i = static_cast<int>((mousePos.y - 10) / cellSize);
            if (i >= 0 && i < boardSize && j >= 0 && j < boardSize) {
                if (board[i][j] == Player::NONE) {
                    auto flips = getFlippableStones(i, j, currentPlayer);
                    if (!flips.empty()) {
                        previewRow = i;
                        previewCol = j;
                        previewFlips = flips;
                    }
                    else {
                        previewRow = -1;
                        previewCol = -1;
                        previewFlips.clear();
                    }
                }
                else {
                    previewRow = -1;
                    previewCol = -1;
                    previewFlips.clear();
                }
            }
        }
        else {
            previewRow = -1;
            previewCol = -1;
            previewFlips.clear();
        }
    }

    // 處理棋盤下棋 (手動操作模式)
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (useAIOpponent && currentPlayer == Player::WHITE)
            return;
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x),
            static_cast<float>(event.mouseButton.y));
        if (mousePos.x >= 10 && mousePos.x <= (10 + boardSize * cellSize) &&
            mousePos.y >= 10 && mousePos.y <= (10 + boardSize * cellSize)) {
            int j = static_cast<int>((mousePos.x - 10) / cellSize);
            int i = static_cast<int>((mousePos.y - 10) / cellSize);
            if (i >= 0 && i < boardSize && j >= 0 && j < boardSize) {
                if (showPass || passActive) {
                    showPass = false;
                    passActive = false;
                }
                if (board[i][j] == Player::NONE) {
                    auto flips = getFlippableStones(i, j, currentPlayer);
                    if (!flips.empty()) {
                        board[i][j] = currentPlayer;
                        lastMoveRow = i;
                        lastMoveCol = j;
                        for (auto& pos : flips) {
                            board[pos.first][pos.second] = currentPlayer;
                        }
                        switchPlayer();
                        previewRow = -1;
                        previewCol = -1;
                        previewFlips.clear();
                    }
                }
            }
        }
    }

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            // 可根據需求添加其他鍵盤事件
        }
    }
}

std::shared_ptr<Scene> GameScene::update(sf::Time delta) {
    // 若按下 Home 按鈕，返回 StartScene (不保留原棋局)
    if (returnHome){
        return std::make_shared<StartScene>(
            font,
            sf::Vector2u(
                static_cast<unsigned int>(background.getSize().x),
                static_cast<unsigned int>(background.getSize().y)
            )
            );
    }

    if (passActive) {
        if (passClock.getElapsedTime().asSeconds() < 1.5f)
            return nullptr;
        else {
            showPass = false;
            passActive = false;
        }
    }
    else {
        // 更新倒數計時
        if (currentPlayer == Player::BLACK) {
            remainingTimeBlack -= delta.asSeconds();
            if (remainingTimeBlack <= 0) {
                remainingTimeBlack = 0;
                isActive = false;
                return std::make_shared<WinScene>(font,
                    sf::Vector2u(static_cast<unsigned int>(background.getSize().x),
                        static_cast<unsigned int>(background.getSize().y)),
                    "Time elapsed, White wins!");
            }
        }
        else { // 輪到白棋
            remainingTimeWhite -= delta.asSeconds();
            if (remainingTimeWhite <= 0) {
                remainingTimeWhite = 0;
                isActive = false;
                return std::make_shared<WinScene>(font,
                    sf::Vector2u(static_cast<unsigned int>(background.getSize().x),
                        static_cast<unsigned int>(background.getSize().y)),
                    "Time elapsed, Black wins!");
            }
        }
    }

    textBlackTimer.setString("Black Timer: " + std::to_string(static_cast<int>(remainingTimeBlack)) + "s");
    textWhiteTimer.setString("White Timer: " + std::to_string(static_cast<int>(remainingTimeWhite)) + "s");

    if (useAIOpponent && currentPlayer == Player::WHITE) {
        std::vector<std::tuple<int, int, std::vector<std::pair<int, int>>>> aiMoves;
        for (int i = 0; i < boardSize; i++) {
            for (int j = 0; j < boardSize; j++) {
                if (board[i][j] == Player::NONE) {
                    auto flips = getFlippableStones(i, j, currentPlayer);
                    if (!flips.empty())
                        aiMoves.push_back(std::make_tuple(i, j, flips));
                }
            }
        }
        if (!aiMoves.empty()) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, aiMoves.size() - 1);
            int chosen = dis(gen);
            int r, c;
            std::vector<std::pair<int, int>> chosenFlips;
            std::tie(r, c, chosenFlips) = aiMoves[chosen];
            board[r][c] = currentPlayer;
            lastMoveRow = r;
            lastMoveCol = c;
            for (auto& pos : chosenFlips)
                board[pos.first][pos.second] = currentPlayer;
            switchPlayer();
            previewRow = -1;
            previewCol = -1;
            previewFlips.clear();
        }
        else {
            Player opponent = (currentPlayer == Player::BLACK) ? Player::WHITE : Player::BLACK;
            if (hasLegalMove(opponent)) {
                if (!passActive) {
                    showPass = true;
                    passActive = true;
                    passClock.restart();
                    switchPlayer();
                }
                return nullptr;
            }
            else {
                int countBlack = 0, countWhite = 0;
                for (int i = 0; i < boardSize; i++) {
                    for (int j = 0; j < boardSize; j++) {
                        if (board[i][j] == Player::BLACK)
                            countBlack++;
                        else if (board[i][j] == Player::WHITE)
                            countWhite++;
                    }
                }
                std::string winner;
                if (countBlack > countWhite)
                    winner = "Black wins!";
                else if (countWhite > countBlack)
                    winner = "White wins!";
                else
                    winner = "Draw!";
                isActive = false;
                return std::make_shared<WinScene>(font,
                    sf::Vector2u(static_cast<unsigned int>(background.getSize().x),
                        static_cast<unsigned int>(background.getSize().y)),
                    winner);
            }
        }
    }
    else {
        if (hasLegalMove(currentPlayer)) {
            showPass = false;
            passActive = false;
        }
        else {
            Player opponent = (currentPlayer == Player::BLACK) ? Player::WHITE : Player::BLACK;
            if (hasLegalMove(opponent)) {
                if (!passActive) {
                    showPass = true;
                    passActive = true;
                    passClock.restart();
                    switchPlayer();
                }
                return nullptr;
            }
            else {
                int countBlack = 0, countWhite = 0;
                for (int i = 0; i < boardSize; i++) {
                    for (int j = 0; j < boardSize; j++) {
                        if (board[i][j] == Player::BLACK)
                            countBlack++;
                        else if (board[i][j] == Player::WHITE)
                            countWhite++;
                    }
                }
                std::string winner;
                if (countBlack > countWhite)
                    winner = "Black wins!";
                else if (countWhite > countBlack)
                    winner = "White wins!";
                else
                    winner = "Draw!";
                isActive = false;
                return std::make_shared<WinScene>(font,
                    sf::Vector2u(static_cast<unsigned int>(background.getSize().x),
                        static_cast<unsigned int>(background.getSize().y)),
                    winner);
            }
        }
    }

    return nullptr;
}

void GameScene::draw(sf::RenderWindow& window) {
    window.draw(background);
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
            window.draw(cells[i][j]);
        }
    }

    // 繪製預覽合法棋步的標記
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
            if (board[i][j] == Player::NONE) {
                auto flips = getFlippableStones(i, j, currentPlayer);
                if (!flips.empty()) {
                    sf::CircleShape marker(cellSize / 6.f);
                    if (currentPlayer == Player::BLACK)
                        marker.setFillColor(sf::Color(0, 0, 0, 128));
                    else if (currentPlayer == Player::WHITE)
                        marker.setFillColor(sf::Color(255, 255, 255, 200));
                    marker.setOrigin(marker.getRadius(), marker.getRadius());
                    sf::Vector2f pos = cells[i][j].getPosition();
                    pos.x += (cellSize - 2) / 2.f;
                    pos.y += (cellSize - 2) / 2.f;
                    marker.setPosition(pos);
                    window.draw(marker);
                }
            }
        }
    }

    // 繪製已落棋子
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
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

    // 繪製滑鼠懸停預覽
    if (previewRow != -1 && previewCol != -1) {
        sf::CircleShape previewPiece(cellSize / 2.f - 4);
        previewPiece.setOrigin(previewPiece.getRadius(), previewPiece.getRadius());
        sf::Vector2f pos = cells[previewRow][previewCol].getPosition();
        pos.x += (cellSize - 2) / 2.f;
        pos.y += (cellSize - 2) / 2.f;
        previewPiece.setPosition(pos);
        if (currentPlayer == Player::BLACK)
            previewPiece.setFillColor(sf::Color(0, 0, 0, 128));
        else if (currentPlayer == Player::WHITE)
            previewPiece.setFillColor(sf::Color(255, 255, 255, 200));
        window.draw(previewPiece);

        for (const auto& flipPos : previewFlips) {
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
                flipPreview.setFillColor(sf::Color(255, 255, 255, 200));
            window.draw(flipPreview);
        }
    }

    // 繪製最後落子位置的紅色三角形
    if (lastMoveRow != -1 && lastMoveCol != -1) {
        sf::ConvexShape triangle;
        triangle.setPointCount(3);
        float markerWidth = cellSize / 3.f;
        float markerHeight = cellSize / 3.f;
        sf::Vector2f cellPos = cells[lastMoveRow][lastMoveCol].getPosition();
        cellPos.x += (cellSize - 2) / 2.f;
        cellPos.y += (cellSize - 2) / 2.f;
        triangle.setPoint(0, sf::Vector2f(cellPos.x, cellPos.y - markerHeight / 2.f));
        triangle.setPoint(1, sf::Vector2f(cellPos.x - markerWidth / 2.f, cellPos.y + markerHeight / 2.f));
        triangle.setPoint(2, sf::Vector2f(cellPos.x + markerWidth / 2.f, cellPos.y + markerHeight / 2.f));
        triangle.setFillColor(sf::Color::Red);
        window.draw(triangle);
    }

    window.draw(textBlackTimer);
    window.draw(textWhiteTimer);
    if (showPass)
        window.draw(passText);

    // 繪製右下角 Home 按鈕 (100×100)
    window.draw(homeButton);
}

void GameScene::switchPlayer() {
    currentPlayer = (currentPlayer == Player::BLACK) ? Player::WHITE : Player::BLACK;
    previewRow = -1;
    previewCol = -1;
    previewFlips.clear();
}