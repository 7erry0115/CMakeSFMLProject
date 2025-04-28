#ifndef WINSCENE_HPP
#define WINSCENE_HPP

#include "../scene/Scene.hpp"
#include "../button/Button.hpp"
#include "../common/Common.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

class WinScene : public Scene {
public:
    // 新增 winner 參數，顯示獲勝者資訊
    WinScene(const sf::Font &font, const sf::Vector2u &windowSize, const std::string &winner);

    virtual void handleEvent(const sf::Event &event) override;

    virtual std::shared_ptr<Scene> update(sf::Time delta) override;

    virtual void draw(sf::RenderWindow &window) override;

private:
    const sf::Font &font;
    sf::RectangleShape background;
    sf::Text winText;
    Button homeButton; // 假設已有 Button 類別實作於相依模組中
};

#endif // WINSCENE_HPP
