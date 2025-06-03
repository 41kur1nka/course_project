#include "MainMenuScreen.h"
#include "../Game.h"
#include <stdexcept>
#include "PauseScreen.h"

MainMenuScreen::MainMenuScreen(GameState& stateRef, Game& gameRef):mState(stateRef), mGame(gameRef){}


void MainMenuScreen::loadAssets(const sf::RenderWindow& window)
{
    // Background 
    if (!mBgTex.loadFromFile("assets/ui/menu_bg.png"));
    else {
        mBgSprite.setTexture(mBgTex);
    }

    // Buttons
    if (!startN.loadFromFile("assets/ui/btn_start_norm.png") ||
        !startH.loadFromFile("assets/ui/btn_start_hover.png") ||
        !startP.loadFromFile("assets/ui/btn_start_down.png"))
    {
        throw std::runtime_error("Failed to load MainMenu button textures");
    }
    if (!scoreN.loadFromFile("assets/ui/btn_scores_norm.png") ||
        !scoreH.loadFromFile("assets/ui/btn_scores_hover.png") ||
        !scoreP.loadFromFile("assets/ui/btn_scores_down.png"))
    {
        throw std::runtime_error("Failed to load MainMenu button textures");
    }

    if (!setN.loadFromFile("assets/ui/btn_settings_norm.png") ||
        !setH.loadFromFile("assets/ui/btn_settings_hover.png") ||
        !setP.loadFromFile("assets/ui/btn_settings_down.png"))
    {
        throw std::runtime_error("Failed to load MainMenu button textures");
    }

    float cx = window.getSize().x / 2.f;
    float buttonHeight = static_cast<float>(startN.getSize().y);
    float totalHeight = buttonHeight * 3 + 20.f * 2;
    float startY = window.getSize().y / 2.f - totalHeight / 2.f;

    mButtons.clear();

    // START GAME
    mButtons.emplace_back(
        startN, startH, startP,
        sf::Vector2f(cx - startN.getSize().x / 2.f, startY),
        [&]() { mGame.resetLogic();
            mState = GameState::Playing; }
    );

    // HIGH SCORES
    mButtons.emplace_back(
        scoreN, scoreH, scoreP,
        sf::Vector2f(cx - scoreN.getSize().x / 2.f, startY + buttonHeight + 20.f),
        [&]() { mState = GameState::HighScores; }
    );

    // SETTINGS
    mButtons.emplace_back(
        setN, setH, setP,
        sf::Vector2f(cx - setN.getSize().x / 2.f, startY + (buttonHeight + 20.f) * 2),
        [&]() { mState = GameState::Settings; }
    );
}

void MainMenuScreen::update(const sf::RenderWindow& window)
{
    for (auto& btn : mButtons)
        btn.update(window);
}

void MainMenuScreen::draw(sf::RenderTarget& target)
{
    // background
    if (mBgTex.getSize().x > 0)
        target.draw(mBgSprite);

    // buttons
    for (auto& btn : mButtons)
        btn.draw(target);
}

