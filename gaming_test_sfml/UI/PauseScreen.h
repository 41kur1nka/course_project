#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "Button.h"
#include "../HighScoresManager.h"
#include "../GameLogic.h"

enum class GameState;

class PauseScreen {
public:
    PauseScreen(GameState& stateRef, HighScoresManager& scoresManager, GameLogic& logicRef);

    void loadAssets(const sf::RenderWindow& window);

    void update(const sf::RenderWindow& window);

    void draw(sf::RenderTarget& target);


private:
    GameState& mState;
    HighScoresManager& mScoresManager;
    GameLogic& mLogic;
    sf::Texture mResumeNorm, mResumeHover, mResumeDown;
    sf::Texture mExitNorm, mExitHover, mExitDown;
    std::vector<Button> mButtons;

    
};