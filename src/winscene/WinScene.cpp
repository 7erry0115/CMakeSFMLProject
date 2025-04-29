#include "WinScene.hpp"
#include "../startscene/StartScene.hpp"

WinScene::WinScene(const sf::Font& font, const sf::Vector2u& windowSize, const std::string& winner)
    : font(font),
    homeButton(font, "Home", 30),
    windowSize(windowSize),
    returnHome(false)
{
    background.setSize(sf::Vector2f(windowSize));
    background.setFillColor(sf::Color(200, 200, 200));

    winText.setFont(font);
    winText.setCharacterSize(30);
    winText.setFillColor(sf::Color::Red);
    winText.setString(winner);
    // Center the winText
    sf::FloatRect textRect = winText.getLocalBounds();
    winText.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
    winText.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f - 50));

    // Set homeButton position (adjust accordingly)
    homeButton.setPosition(sf::Vector2f(windowSize.x / 2.f - 75, windowSize.y / 2.f + 20));
}

void WinScene::handleEvent(const sf::Event& event) {
    // Handle mouse clicks on the home button or press of Home key
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x),
            static_cast<float>(event.mouseButton.y));
        if (homeButton.contains(mousePos)) {
            returnHome = true;
        }
    }
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Home) {
            returnHome = true;
        }
    }
}

std::shared_ptr<Scene> WinScene::update(sf::Time delta) {
    // If the home flag is set, return a new StartScene
    if (returnHome)
        return std::make_shared<StartScene>(font, windowSize);
    return nullptr;
}

void WinScene::draw(sf::RenderWindow& window) {
    window.draw(background);
    window.draw(winText);
    // Draw the homeButton using its public draw method accepting the RenderWindow
    homeButton.draw(window);
}