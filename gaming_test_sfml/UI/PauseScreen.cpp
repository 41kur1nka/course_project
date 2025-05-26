#include "PauseScreen.h"
#include "../Game.h"
#include <stdexcept>

PauseScreen::PauseScreen(GameState& stateRef)
    : mState(stateRef)
{
}

void PauseScreen::loadAssets(const sf::RenderWindow& window)
{
    // Замените пути к ассетам на ваши!
    if (!mResumeNorm.loadFromFile("assets/ui/btn_resume_norm.png") ||
        !mResumeHover.loadFromFile("assets/ui/btn_resume_hover.png") ||
        !mResumeDown.loadFromFile("assets/ui/btn_resume_down.png"))
        throw std::runtime_error("Can't load resume button textures");
    if (!mExitNorm.loadFromFile("assets/ui/btn_exit_norm.png") ||
        !mExitHover.loadFromFile("assets/ui/btn_exit_hover.png") ||
        !mExitDown.loadFromFile("assets/ui/btn_exit_down.png"))
        throw std::runtime_error("Can't load exit button textures");

    float cx = window.getSize().x / 2.f;
    float cy = window.getSize().y / 2.f;
    float gap = 30.f;
    float bw = static_cast<float>(mResumeNorm.getSize().x);
    float bh = static_cast<float>(mResumeNorm.getSize().y);

    mButtons.clear();
    // Resume
    mButtons.emplace_back(
        mResumeNorm, mResumeHover, mResumeDown,
        sf::Vector2f(cx - bw / 2, cy - bh - gap),
        [&]() { mState = GameState::Playing; }
    );
    // Exit to Menu
    mButtons.emplace_back(
        mExitNorm, mExitHover, mExitDown,
        sf::Vector2f(cx - bw / 2, cy + gap),
        [&]() { mState = GameState::MainMenu; }
    );
}

void PauseScreen::update(const sf::RenderWindow& window)
{
    for (auto& btn : mButtons)
        btn.update(window);
}

void PauseScreen::draw(sf::RenderTarget& target)
{
    for (auto& btn : mButtons)
        btn.draw(target);
}
