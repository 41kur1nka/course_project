#pragma once
#include <SFML/Graphics.hpp>
#include "GameLogic.h"

class GameRenderer {
public:
    GameRenderer(sf::RenderWindow& wnd, GameLogic& logic)
        : mWindow(wnd), mLogic(logic) {

    }

    void render();

private:
    sf::RenderWindow& mWindow;
    GameLogic& mLogic;


    sf::RectangleShape   mPauseOverlay;
    sf::Text             mPauseText;
    sf::Font           mFont;
};
