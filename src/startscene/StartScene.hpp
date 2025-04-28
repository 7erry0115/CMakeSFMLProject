#ifndef STARTSCENE_HPP
#define STARTSCENE_HPP

#include "../scene/Scene.hpp"
#include "../button/Button.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

class StartScene : public Scene {
public:
    StartScene(const sf::Font &font, const sf::Vector2u &windowSize);

    virtual void handleEvent(const sf::Event &event) override;

    virtual std::shared_ptr<Scene> update(sf::Time delta) override;

    virtual void draw(sf::RenderWindow &window) override;

private:
    const sf::Font &font;
    sf::RectangleShape background;
    Button aiButton;
    Button onlineButton;

    // 初始界面中的模式選擇
    enum class Mode { NONE, AI, ONLINE };

    Mode selectedMode;
};

#endif // STARTSCENE_HPP
