#ifndef WINSCENE_HPP
#define WINSCENE_HPP

#include "../scene/Scene.hpp"
#include "../button/Button.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

class WinScene : public Scene {
public:
    // The constructor now takes the window size and winner text (in English)
    WinScene(const sf::Font& font, const sf::Vector2u& windowSize, const std::string& winner);

    virtual void handleEvent(const sf::Event& event) override;
    virtual std::shared_ptr<Scene> update(sf::Time delta) override;
    virtual void draw(sf::RenderWindow& window) override;

private:
    const sf::Font& font;
    sf::RectangleShape background;
    sf::Text winText;
    Button homeButton;
    sf::Vector2u windowSize;

    // When set to true, the scene will switch back to the StartScene
    bool returnHome;
};

#endif // WINSCENE_HPP