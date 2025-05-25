#pragma once
#include <SFML/Graphics.hpp>
#include "GameLogic.h"

class GameRenderer {
public:
    GameRenderer(sf::RenderWindow& wnd, GameLogic& logic);

    void render();
private:
    sf::RenderWindow& mWindow;
    GameLogic& mLogic;

    // Äëÿ HUD
    sf::Font                mFont;
    sf::Text                mScoreText;
    sf::Text                mTimeText;
    sf::Text                mActiveText;    
};
