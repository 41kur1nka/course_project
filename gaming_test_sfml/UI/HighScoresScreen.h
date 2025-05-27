#pragma once

#include <SFML/Graphics.hpp>
#include "../HighScoresManager.h"
#include "../Game.h"
#include "Button.h"

enum class GameState;

class HighScoresScreen {
public:
    HighScoresScreen(GameState& stateRef, HighScoresManager& manager);

    void loadAssets(const sf::RenderWindow& window);
    void update(const sf::RenderWindow& window);
    void draw(sf::RenderTarget& target);

private:
    GameState& mState;
    HighScoresManager& mScores;
    sf::Font mFont;
    sf::Text mTitleText;
    sf::Text mEntryText;

    sf::Texture mBtnNorm, mBtnHover, mBtnDown;
    std::unique_ptr<Button> mBackButton;

};
