#include "Button.hpp"

Button::Button() {
    // 預設建構子，初始化預設外觀
    background.setFillColor(sf::Color::Blue);
    label.setFillColor(sf::Color::White);
}

Button::Button(const sf::Font& font, const std::string& text, unsigned int characterSize)
    : label(text, font, characterSize)
{
    background.setFillColor(sf::Color::Blue);
    // 預設的 Button 大小，可依需求調整
    background.setSize(sf::Vector2f(150.f, 50.f));
}

void Button::setPosition(const sf::Vector2f& position) {
    background.setPosition(position);
    // 中心置中 label
    sf::FloatRect textRect = label.getLocalBounds();
    label.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
    label.setPosition(position.x + background.getSize().x / 2.f, position.y + background.getSize().y / 2.f);
}

void Button::setSize(const sf::Vector2f& size) {
    background.setSize(size);
}

void Button::setText(const std::string& text) {
    label.setString(text);
}

bool Button::contains(const sf::Vector2f& point) const {
    return background.getGlobalBounds().contains(point);
}

void Button::setBackgroundColor(const sf::Color& color) {
    background.setFillColor(color);
}

void Button::setTextColor(const sf::Color& color) {
    label.setFillColor(color);
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(background, states);
    target.draw(label, states);
}

// 新增這個公共 draw 方法，方便外部直接傳入 RenderWindow
void Button::draw(sf::RenderWindow& window) const {
    window.draw(*this);
}