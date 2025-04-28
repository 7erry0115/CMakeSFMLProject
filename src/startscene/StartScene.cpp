#include "../startscene/StartScene.hpp"
#include "../gamescene/GameScene.hpp"
#include <iostream>

StartScene::StartScene(const sf::Font &font, const sf::Vector2u &windowSize)
    : font(font), selectedMode(Mode::NONE),
      aiButton(font, u8"AI Opponent"),
      onlineButton(font, "Online Opponent") {
    // 設定背景（例如淡藍色）
    background.setSize(sf::Vector2f(windowSize));
    background.setFillColor(sf::Color(200, 220, 255));

    // 設定按鈕尺寸與位置（置中，位於畫面下方）
    sf::Vector2f btnSize(200.f, 60.f);
    aiButton.setSize(btnSize);
    onlineButton.setSize(btnSize);

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
    // 當按下按鈕後切換至遊戲介面（可依 selectedMode 做不同處理）
    if (selectedMode != Mode::NONE) {
        return std::make_shared<GameScene>(font, sf::Vector2u(800, 600));
    }
    return nullptr;
}

void StartScene::draw(sf::RenderWindow &window) {
    window.draw(background);
    window.draw(aiButton);
    window.draw(onlineButton);
}
