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
    auto initText = [&](sf::Text& t, unsigned size) {
        t.setFont(mFont);
        t.setCharacterSize(size);
        t.setFillColor(sf::Color::White);
        };
    initText(mScoreText, 20);
    initText(mTimeText, 20);
    initText(mActiveText, 20);


    // === Interaction Screen setup ===
    // Прозрачный полупрозрачный фон на весь экран
    mInteractionOverlay.setSize(
        sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
    mInteractionOverlay.setFillColor(sf::Color(0, 0, 0, 180));

    // Текст подсказки
    mInteractionText.setFont(mFont);
    mInteractionText.setCharacterSize(36);
    mInteractionText.setFillColor(sf::Color::White);
    mInteractionText.setOutlineColor(sf::Color::Black);
    mInteractionText.setOutlineThickness(2.f);
    mInteractionText.setString("Press E to continue...");

    // Центрируем origin и позицию
    {
        // временно в (0,0), чтобы получить globalBounds
        mInteractionText.setPosition(0.f, 0.f);
        auto gb = mInteractionText.getGlobalBounds();
        mInteractionText.setPosition(
            window.getSize().x / 2.f - gb.width / 2.f,
            window.getSize().y / 2.f - gb.height / 2.f
        );
    }
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

    if (mLogic.isInteracting()) {
        mWindow.draw(mInteractionOverlay);
        mWindow.draw(mInteractionText);
    }

}
