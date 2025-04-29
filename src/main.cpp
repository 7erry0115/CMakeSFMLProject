#include <SFML/Graphics.hpp>
#include "common/Common.hpp"       // 使用共用的 Player 定義
#include "startscene/StartScene.hpp"
#include "gamescene/GameScene.hpp"
#include "winscene/WinScene.hpp"
#include "scene/Scene.hpp"
#include "button/Button.hpp"
#include <memory>
#include <iostream>

int main() {
    /* 先執行文字版遊戲
    std::cout << "歡迎來到黑白棋 (文字版)!" << std::endl;
    system("pause");*/

    // 文字版遊戲邏輯（略）：請確保該部分已正確實作

    // 例如：假設 Othello 類別已定義且運作正常
    // Othello game;
    // game.play();

    // 新增：執行 SFML 版本
    sf::VideoMode videoMode(800, 600);
    sf::RenderWindow window(videoMode, "Reversi", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    // 載入字型 (請確認字型檔案路徑是否正確，例如 "assets/arial.ttf")
    sf::Font font;
    if (!font.loadFromFile("assets/微軟正黑體.ttf")) {
        std::cerr << "無法載入字型檔 assets/arial.ttf" << std::endl;
        return -1;
    }

    // 改成從 StartScene 進入，StartScene 中會提供兩個按鈕讓玩家選擇 AI 對手或線上對戰，
    // 當玩家選擇後會回傳使用三個引數的新 GameScene。
    std::shared_ptr<Scene> currentScene = std::make_shared<StartScene>(font, window.getSize());

    sf::Clock clock;
    while (window.isOpen()) {
        sf::Time delta = clock.restart();
        sf::Event event;
        while (window.pollEvent(event)) {
            // 點選視窗右上角 X 按鈕關閉視窗
            if (event.type == sf::Event::Closed)
                window.close();

            // 當按下 Home 鍵時回到起始畫面
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Home) {
                currentScene = std::make_shared<StartScene>(font, window.getSize());
                continue;
            }

            currentScene->handleEvent(event);
        }
        if (auto nextScene = currentScene->update(delta)) {
            currentScene = nextScene;
        }
        window.clear();
        currentScene->draw(window);
        window.display();
    }
    return 0;
}
