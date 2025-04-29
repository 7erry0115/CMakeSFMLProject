#ifndef STARTSCENE_HPP
#define STARTSCENE_HPP

#include "../scene/Scene.hpp"
#include "../button/Button.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

class StartScene : public Scene {
public:
    // 构造式接受字型與視窗大小參數
    StartScene(const sf::Font& font, const sf::Vector2u& windowSize);

    virtual void handleEvent(const sf::Event& event) override;
    virtual std::shared_ptr<Scene> update(sf::Time delta) override;
    virtual void draw(sf::RenderWindow& window) override;

private:
    const sf::Font& font;
    // 新增：背景圖片所用的 Texture 與 Sprite
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    // 若無使用背景圖片可保留此 RectangleShape(也可以刪除)
    sf::RectangleShape background;

    Button aiButton;
    Button onlineButton;

    // 使用者所選的模式
    enum class Mode { NONE, AI, ONLINE };
    Mode selectedMode;
};

#endif // STARTSCENE_HPP