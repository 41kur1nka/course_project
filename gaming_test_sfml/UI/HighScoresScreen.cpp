#include "../Game.h"
#include <stdexcept>
#include <sstream>
#include "HighScoresScreen.h"
#include <iostream>

HighScoresScreen::HighScoresScreen(GameState& stateRef, HighScoresManager& manager)
    : mState(stateRef), mScores(manager),
    mBackButton(nullptr)
{
}

void HighScoresScreen::loadAssets(const sf::RenderWindow& window)
{
    if (!mFont.loadFromFile("assets/fonts/Montserrat-Black.ttf"))
        throw std::runtime_error("Failed to load font for HighScores");

    mTitleText.setFont(mFont);
    mTitleText.setCharacterSize(36);
    mTitleText.setFillColor(sf::Color::White);
    mTitleText.setString("HIGH SCORES");
    mTitleText.setPosition(window.getSize().x / 2.f - 120, 30);

    mEntryText.setFont(mFont);
    mEntryText.setCharacterSize(24);
    mEntryText.setFillColor(sf::Color::White);

    //  нопка back
    if(!mBtnNorm.loadFromFile("assets/ui/btn_gamemenu_norm.png")||
    !mBtnHover.loadFromFile("assets/ui/btn_gamemenu_hover.png")||
    !mBtnDown.loadFromFile("assets/ui/btn_gamemenu_down.png"))
        throw std::runtime_error("Can't load back button textures");
    // позици€ в конструкторе
    float btnX = window.getSize().x / 2.f - mBtnNorm.getSize().x / 2.f;
    float btnY = window.getSize().y - mBtnNorm.getSize().y - 60.f;

    mBackButton = std::make_unique<Button>(
        mBtnNorm, mBtnHover, mBtnDown,
        sf::Vector2f(btnX, btnY),
        [this]() { mState = GameState::MainMenu; }
    );
}

void HighScoresScreen::loadScoresOnline()
{
    mCachedScores = mScores.getScoresOnline();
    mScoresLoaded = true;
}



void HighScoresScreen::update(const sf::RenderWindow& window)
{
    if (mBackButton) mBackButton->update(window);

}

void HighScoresScreen::draw(sf::RenderTarget& target)
{
    target.draw(mTitleText);

    // –исуем только если mScoresLoaded!
    float y = 150.f;
    for (size_t i = 0; i < mCachedScores.size() && i < 10; ++i) {
        sf::Text text;
        text.setFont(mFont);
        text.setString(mCachedScores[i].first + ": " + std::to_string(mCachedScores[i].second));
        text.setCharacterSize(28);
        text.setFillColor(sf::Color::White);
        text.setPosition(120.f, y);
        target.draw(text);
        y += 40.f;
    }

    if (mBackButton) mBackButton->draw(target);
}

