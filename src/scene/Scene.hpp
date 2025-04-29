#ifndef SCENE_HPP
#define SCENE_HPP

#include <SFML/Graphics.hpp>
#include <memory>

// 介面狀態基底類別
class Scene {
public:
    virtual ~Scene() = default;
    // 處理事件
    virtual void handleEvent(const sf::Event& event) = 0;
    // 更新邏輯，回傳下個場景 (若不切換則回傳自身)
    virtual std::shared_ptr<Scene> update(sf::Time delta) = 0;
    // 繪製畫面
    virtual void draw(sf::RenderWindow& window) = 0;
};

#endif // SCENE_HPP
