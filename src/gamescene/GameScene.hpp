#ifndef GAMESCENE_HPP
#define GAMESCENE_HPP

#include "../scene/Scene.hpp"
#include "../button/Button.hpp"
#include "../common/Common.hpp"  // Common.hpp 定義 Player (例如 Player::BLACK, Player::WHITE, Player::NONE)
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class GameScene : public Scene {
public:
    static constexpr int boardSize = 8; // 棋盤為8x8
    // 建構式：第三個參數 useAIOpponent，true 表示白棋由 AI 控制，false 表示雙方皆由玩家操控
    GameScene(const sf::Font &font, const sf::Vector2u &windowSize, bool useAIOpponent);

    virtual void handleEvent(const sf::Event &event) override; // 處理事件
    virtual std::shared_ptr<Scene> update(sf::Time delta) override; // 更新場景
    virtual void draw(sf::RenderWindow &window) override; // 繪製場景

private:
    const sf::Font &font; // 字型參考
    sf::RectangleShape background; // 背景矩形
    float cellSize; // 棋盤中每格的大小
    std::vector<std::vector<sf::RectangleShape> > cells; // 棋盤上各格
    std::vector<std::vector<Player> > board; // 棋盤狀態

    // 兩邊倒數計時 (秒)
    float remainingTimeBlack;
    float remainingTimeWhite;
    sf::Text textBlackTimer; // 黑棋倒數文字
    sf::Text textWhiteTimer; // 白棋倒數文字

    Player currentPlayer; // 當前落子玩家

    // 預覽棋步相關變數
    int previewRow;
    int previewCol;
    std::vector<std::pair<int, int> > previewFlips; // 預覽要翻面的棋子

    // PASS 狀態變數
    bool isActive;
    sf::Text passText;
    bool showPass;
    bool passActive;
    sf::Clock passClock;

    // 記錄最後落子位置 (以紅色三角形標示)
    int lastMoveRow;
    int lastMoveCol;

    // 當 useAIOpponent 為 true 時，白棋由 AI 控制；否則由玩家操作
    bool useAIOpponent;

    // 用來檢查是否按下 Home 按鈕
    bool returnHome;
    Button homeButton; // Home 按鈕

    // 切換玩家並清除預覽資料
    void switchPlayer();

    // 取得一定落子位置可翻面的對手棋子列表
    std::vector<std::pair<int, int> > getFlippableStones(int row, int col, Player player) const;

    // 檢查該玩家是否至少有一個合法棋步
    bool hasLegalMove(Player player) const;
};

#endif // GAMESCENE_HPP
