#include "MainMenuScreen.h"
#include "../Game.h"
#include <stdexcept>

MainMenuScreen::MainMenuScreen(GameState& stateRef):mState(stateRef){}

void MainMenuScreen::loadAssets()
{
    // ќпционально: фон
    if (!mBgTex.loadFromFile("assets/ui/menu_bg.png"))
        ; // если нет фона Ч не фатально
    else {
        mBgSprite.setTexture(mBgTex);
    }

    //  нопки (примерные пути Ч договоритесь с художником)
    if (!startN.loadFromFile("assets/ui/btn_start_norm.png") ||
        !startH.loadFromFile("assets/ui/btn_start_hover.png") ||
        !startP.loadFromFile("assets/ui/btn_start_down.png"))
    {
        throw std::runtime_error("Failed to load MainMenu button textures");
    }
    if (!scoreN.loadFromFile("assets/ui/btn_gamemenu_norm.png") ||
        !scoreH.loadFromFile("assets/ui/btn_gamemenu_hover.png") ||
        !scoreP.loadFromFile("assets/ui/btn_gamemenu_down.png"))
    {
        throw std::runtime_error("Failed to load MainMenu button textures");
    }

    // ѕозиции кнопок Ч отступаем от центра
    float cx = mBgTex.getSize().x ? float(mBgTex.getSize().x) / 2.f: 400.f; // центр окна X
    float y = 200.f; // стартова€ Y-позици€

    // START GAME
    mButtons.emplace_back(
        startN, startH, startP,
        sf::Vector2f(cx - startN.getSize().x / 2.f, y),
        [&]() { mState = GameState::Playing; }
    );

    // HIGH SCORES
    mButtons.emplace_back(
        scoreN, scoreH, scoreP,
        sf::Vector2f(cx - scoreN.getSize().x / 2.f, y + 100.f),
        [&]() { mState = GameState::HighScores; }
    );

    // SETTINGS
    mButtons.emplace_back(
        startN, startH, startP,
        sf::Vector2f(cx - startN.getSize().x / 2.f, y + 200.f),
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
    // фон
    if (mBgTex.getSize().x > 0)
        target.draw(mBgSprite);

    // кнопки
    for (auto& btn : mButtons)
        btn.draw(target);
}

