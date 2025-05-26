#pragma once
#include <SFML/Graphics.hpp>
#include "GameLogic.h"
#include <SFML/Graphics/RectangleShape.hpp>

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

    sf::RectangleShape mInteractionOverlay;
    sf::Text           mInteractionText;

    // QTE
    sf::Text mQTEText;
};
