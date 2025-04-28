#ifndef GAMESCENE_HPP
#define GAMESCENE_HPP
#include "../scene/Scene.hpp"
#include "../button/Button.hpp"
#include "../common/Common.hpp"  // Common.hpp 中定義了 Player
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class GameScene : public Scene {
public:
    static constexpr int boardSize = 8;

    GameScene(const sf::Font &font, const sf::Vector2u &windowSize);

    virtual void handleEvent(const sf::Event &event) override;

    virtual std::shared_ptr<Scene> update(sf::Time delta) override;

    virtual void draw(sf::RenderWindow &window) override;

private:
    const sf::Font &font;
    sf::RectangleShape background;
    float cellSize;
    std::vector<std::vector<sf::RectangleShape> > cells;
    std::vector<std::vector<Player> > board;

    // 雙玩家獨立倒數時間 (單位：秒)
    float remainingTimeBlack;
    float remainingTimeWhite;

    sf::Text textBlackTimer;
    sf::Text textWhiteTimer;

    Player currentPlayer;

    // 新增：預覽棋步所需的變數，初始值 -1 表示無預覽
    int previewRow;
    int previewCol;
    std::vector<std::pair<int, int> > previewFlips;

    // 新增場景有效性檢查旗標
    bool isActive;

    // 切換回合，不重置倒數時間 (若跳步則在 update 中呼叫)
    void switchPlayer();

    // 傳回在 (row, col) 落子後，可翻轉的對方棋子位置列表
    std::vector<std::pair<int, int> > getFlippableStones(int row, int col, Player player) const;

    // 判定指定玩家是否有至少一步合法棋步
    bool hasLegalMove(Player player) const;
};

#endif // GAMESCENE_HPP
