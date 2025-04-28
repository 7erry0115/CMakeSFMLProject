#include "../winscene/WinScene.hpp"
#include "../startscene/StartScene.hpp"
#include <iostream>

WinScene::WinScene(const sf::Font &font, const sf::Vector2u &windowSize, const std::string &winner)
    : font(font),
      homeButton(font, u8"Home", 30) // 使用 u8 前綴確認中文字串採用 UTF-8
{
    background.setSize(sf::Vector2f(windowSize));
    background.setFillColor(sf::Color(200, 200, 200));

    winText.setFont(font);
    winText.setCharacterSize(30);
    winText.setFillColor(sf::Color::Red);
    // 這裡 winner 參數請以 UTF-8 編碼傳入，例如 u8"玩家白獲勝!" 或 u8"玩家黑獲勝!"
    winText.setString(winner);
    // 置中 winText
    sf::FloatRect textRect = winText.getLocalBounds();
    winText.setOrigin(textRect.left + textRect.width / 2.0f,
                      textRect.top + textRect.height / 2.0f);
    winText.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f - 50));

    // 設定 homeButton 位置 (請依實際 Button 實作檢查 setPosition 是否生效)
    homeButton.setPosition(sf::Vector2f(windowSize.x / 2.f - 75, windowSize.y / 2.f + 20));
}

void WinScene::handleEvent(const sf::Event &event) {
    // 處理按鈕點擊事件 (依照實際 Button 實作調整)
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        if (homeButton.contains(mousePos)) {
            // 例如返回首頁場景 (根據專案機制進行場景切換)
            std::shared_ptr<Scene> startScene = std::make_shared<StartScene>(font, sf::Vector2u(800, 600));
            // 此處應進行場景切換，請依照實際機制調整
        }
    }
}

std::shared_ptr<Scene> WinScene::update(sf::Time delta) {
    // 可依需要新增動態效果，此處保留現有設計
    return nullptr;
}

void WinScene::draw(sf::RenderWindow &window) {
    window.draw(background);
    window.draw(winText);
    // 呼叫 Button::draw() 時傳入 RenderWindow 參數
    homeButton.draw(window);
}
