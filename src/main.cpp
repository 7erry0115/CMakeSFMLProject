#include <SFML/Graphics.hpp>
#include "common/Common.hpp"       // 使用共用的 Player 定義
#include "startscene/StartScene.hpp"
#include "gamescene/GameScene.hpp"
#include "winscene/WinScene.hpp"
#include "scene/Scene.hpp"
#include "button/Button.hpp"
#include <memory>
#include <iostream>
#include <vector>

// 注意：原本重複定義的 enum class Player 已移除

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
    sf::RenderWindow window(videoMode, "Othello", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    // 載入字型 (請確認字型檔案路徑是否正確，例如 "assets/arial.ttf")
    sf::Font font;
    if (!font.loadFromFile("assets/font.ttf")) {
        std::cerr << "無法載入字型檔 assets/font.ttf" << std::endl;
        return -1;
    }

    // 以 StartScene 作為起始場景
    std::shared_ptr<Scene> currentScene = std::make_shared<StartScene>(font, window.getSize());

    sf::Clock clock;
    while (window.isOpen()) {
        sf::Time delta = clock.restart();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
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
