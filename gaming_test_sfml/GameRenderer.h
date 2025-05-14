#pragma once
#include <SFML/Graphics.hpp>
#include "GameLogic.h"

class GameRenderer {
public:
    GameRenderer(sf::RenderWindow& wnd, GameLogic& logic);

    void render();
    void handleMouseClick(const sf::Vector2f& mousePos, sf::RenderWindow& window);
private:
    sf::RenderWindow& mWindow;
    GameLogic& mLogic;

    //For pause
    sf::RectangleShape mPauseOverlay;
    sf::Text mPauseText;
    
    //For button pause
    sf::RectangleShape mResumeBtn;
    sf::Text mResumeText;
    sf::RectangleShape mExitBtn;
    sf::Text mExitText;

    sf::Font mFont;
    
};
