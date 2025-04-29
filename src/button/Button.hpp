#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <SFML/Graphics.hpp>
#include <string>

class Button : public sf::Drawable {
public:
    Button();

    Button(const sf::Font &font, const std::string &text, unsigned int characterSize = 24);

    void setPosition(const sf::Vector2f &position);

    void setSize(const sf::Vector2f &size);

    void setText(const std::string &text);

    bool contains(const sf::Vector2f &point) const;

    // 可選擇設定背景顏色與文字顏色
    void setBackgroundColor(const sf::Color &color);

    void setTextColor(const sf::Color &color);

    // 新增一個 public 方法，方便外部傳入 render window 進行繪製
    void draw(sf::RenderWindow &window) const;

private:
    sf::RectangleShape background;
    sf::Text label;

    // 此函式會被 sf::RenderWindow::draw() 呼叫
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};

#endif // BUTTON_HPP
