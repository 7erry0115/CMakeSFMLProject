#include "StartScene.hpp"
#include "../gamescene/GameScene.hpp"
#include <iostream>
#include <filesystem>

StartScene::StartScene(const sf::Font &font, const sf::Vector2u &windowSize)
    : font(font), selectedMode(Mode::NONE)
      , aiButton(font, u8"AI Opponent")
      , onlineButton(font, "Online Opponent") {
    // 顯示目前工作目錄，確認圖片放置的位置是否正確
    std::cout << "工作目錄: " << std::filesystem::current_path() << std::endl;

    // 嘗試載入背景圖片 "reversi_picture.png"
    if (!backgroundTexture.loadFromFile("assets/reversi_picture.png")) {
        std::cerr << "Error loading reversi_picture.png" << std::endl;
        // 載入失敗時備用背景為單色
        background.setSize(sf::Vector2f(windowSize));
        background.setFillColor(sf::Color(200, 220, 255));
    } else {
        backgroundSprite.setTexture(backgroundTexture);
        // 將背景圖片縮放至視窗大小
        sf::Vector2u texSize = backgroundTexture.getSize();
        float scaleX = static_cast<float>(windowSize.x) / texSize.x;
        float scaleY = static_cast<float>(windowSize.y) / texSize.y;
        backgroundSprite.setScale(scaleX, scaleY);
    }

    // 設定備用背景（只是當圖片載入失敗時使用）
    background.setSize(sf::Vector2f(windowSize));
    background.setFillColor(sf::Color(200, 220, 255));

    // 設定按鈕尺寸與位置 (置中，位於畫面下方)
    sf::Vector2f btnSize(200.f, 60.f);
    aiButton.setSize(btnSize);
    onlineButton.setSize(btnSize);

    // 在這裡設置按鈕的背景色與文字顏色
    aiButton.setBackgroundColor(sf::Color(222, 184, 135));
    aiButton.setTextColor(sf::Color::Black);

    onlineButton.setBackgroundColor(sf::Color(222, 184, 135));
    onlineButton.setTextColor(sf::Color::Black);

    float spacing = 20.f;
    float totalWidth = btnSize.x * 2 + spacing;
    float startX = (windowSize.x - totalWidth) / 2.f;
    float posY = windowSize.y - btnSize.y - 50.f;

    aiButton.setPosition(sf::Vector2f(startX, posY));
    onlineButton.setPosition(sf::Vector2f(startX + btnSize.x + spacing, posY));
}

void StartScene::handleEvent(const sf::Event &event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        if (aiButton.contains(mousePos))
            selectedMode = Mode::AI;
        else if (onlineButton.contains(mousePos))
            selectedMode = Mode::ONLINE;
    }
}

std::shared_ptr<Scene> StartScene::update(sf::Time delta) {
    // 當按下按鈕後，根據按鈕選擇建立新 GameScene
    if (selectedMode != Mode::NONE) {
        bool useAIOpponent = (selectedMode == Mode::AI);
        return std::make_shared<GameScene>(font, sf::Vector2u(800, 600), useAIOpponent);
    }
    return nullptr;
}

void StartScene::draw(sf::RenderWindow &window) {
    // 若背景圖片載入成功則繪製背景圖片，否則使用備用單色背景
    if (backgroundTexture.getSize().x > 0)
        window.draw(backgroundSprite);
    else
        window.draw(background);

    window.draw(aiButton);
    window.draw(onlineButton);
}
