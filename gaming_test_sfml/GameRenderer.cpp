#include "GameRenderer.h"
#include <iomanip>
#include <sstream>
#include <stdexcept>

GameRenderer::GameRenderer(sf::RenderWindow& window, GameLogic& logic)
    : mWindow(window)
    , mLogic(logic)
{
    // Загружаем шрифт для HUD
    if (!mFont.loadFromFile("assets/fonts/Montserrat-Black.ttf"))
        throw std::runtime_error("Failed to load font for HUD");

    // Настраиваем три текстовых поля
    auto initText = [&](sf::Text& t) {
        t.setFont(mFont);
        t.setCharacterSize(20);
        t.setFillColor(sf::Color::White);
        };
    initText(mScoreText);
    initText(mTimeText);
    initText(mActiveText);
}

void GameRenderer::render()
{
    // 1) Отрисовка сцены
    // Карта
    mWindow.draw(mLogic.getMap());

    // Инциденты
    for (auto& inc : mLogic.getIncidents())
        inc->render(mWindow);

    // Игрок
    mLogic.getPlayer().render(mWindow);

    // 2) Отрисовка HUD
    // 2.1 Счёт
    mScoreText.setString("Score: " + std::to_string(mLogic.getScore()));
    mScoreText.setPosition(10.f, 10.f);
    mWindow.draw(mScoreText);

    // 2.2 Время (mm:ss)
    {
        int total = static_cast<int>(mLogic.getElapsedSeconds());
        int mm = total / 60;
        int ss = total % 60;
        std::ostringstream oss;
        oss << "Time: "
            << std::setw(2) << std::setfill('0') << mm
            << ":"
            << std::setw(2) << std::setfill('0') << ss;
        mTimeText.setString(oss.str());
    }
    mTimeText.setPosition(10.f, 40.f);
    mWindow.draw(mTimeText);

    // 2.3 Активные инциденты
    mActiveText.setString("Active: " + std::to_string(mLogic.getActiveIncidents()));
    mActiveText.setPosition(10.f, 70.f);
    mWindow.draw(mActiveText);
}
