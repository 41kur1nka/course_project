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

    if(!mBtnNorm.loadFromFile("assets/ui/btn_gamemenu_norm.png")||
    !mBtnHover.loadFromFile("assets/ui/btn_gamemenu_hover.png")||
    !mBtnDown.loadFromFile("assets/ui/btn_gamemenu_down.png"))
        throw std::runtime_error("Can't load back button textures");

    float btnX = window.getSize().x / 2.f - mBtnNorm.getSize().x / 2.f;
    float btnY = window.getSize().y - mBtnNorm.getSize().y - 60.f;

    mBackButton = std::make_unique<Button>(
        mBtnNorm, mBtnHover, mBtnDown,
        sf::Vector2f(btnX, btnY),
        [this]() { mState = GameState::MainMenu; }
    );

    loadScoresOnline();
}

void HighScoresScreen::loadScoresOnline()
{
    auto onlineScores = mScores.getScoresOnline();
    if (!onlineScores.empty()) {
        // Filtering — only the best results by name
        std::map<std::string, int> uniqueScores;
        for (const auto& p : onlineScores) {
            if (uniqueScores.count(p.first)) {
                uniqueScores[p.first] = std::max(uniqueScores[p.first], p.second);
            }
            else {
                uniqueScores[p.first] = p.second;
            }
        }
        mCachedScores.clear();
        for (const auto& p : uniqueScores)
            mCachedScores.emplace_back(p.first, p.second);
        std::sort(mCachedScores.begin(), mCachedScores.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });
    }
    else {
        mCachedScores = mScores.getAll();
    }
    mScoresLoaded = true;
}

void HighScoresScreen::update(const sf::RenderWindow& window)
{
    if (mBackButton) mBackButton->update(window);

    static float updateTimer = 0.f;
    static const float UPDATE_INTERVAL = 5.f;

    updateTimer += 1.f/60.f;
    if (updateTimer >= UPDATE_INTERVAL) {
        loadScoresOnline();
        updateTimer = 0.f;
    }
}

void HighScoresScreen::draw(sf::RenderTarget& target)
{
    sf::RectangleShape background(sf::Vector2f(target.getSize()));
    background.setFillColor(sf::Color(20, 20, 40));
    target.draw(background);

    target.draw(mTitleText);

    if (!mScoresLoaded || mCachedScores.empty()) {
        sf::Text noScoresText;
        noScoresText.setFont(mFont);
        noScoresText.setString("No scores yet!\nBe the first to set a record!");
        noScoresText.setCharacterSize(28);
        noScoresText.setFillColor(sf::Color::White);
        noScoresText.setPosition(
            target.getSize().x / 2.f - noScoresText.getLocalBounds().width / 2.f,
            target.getSize().y / 2.f - noScoresText.getLocalBounds().height / 2.f
        );
        target.draw(noScoresText);
    } else {
        float y = 150.f;
        for (size_t i = 0; i < mCachedScores.size() && i < 10; ++i) {
            std::string place = std::to_string(i + 1) + ". ";
            std::string scoreText = place + mCachedScores[i].first + ": " + 
                                  std::to_string(mCachedScores[i].second);

            sf::Text text;
            text.setFont(mFont);
            text.setString(scoreText);
            text.setCharacterSize(28);
            
            if (i == 0) text.setFillColor(sf::Color(255, 215, 0));
            else if (i == 1) text.setFillColor(sf::Color(192, 192, 192));
            else if (i == 2) text.setFillColor(sf::Color(205, 127, 50));
            else text.setFillColor(sf::Color::White);

            text.setPosition(
                target.getSize().x / 2.f - text.getLocalBounds().width / 2.f,
                y
            );
            target.draw(text);
            y += 40.f;
        }
    }

    if (mBackButton) mBackButton->draw(target);
}

