#include "GameRenderer.h"
#include <stdexcept>

GameRenderer::GameRenderer(sf::RenderWindow& window, GameLogic& logic)
    : mWindow(window)
    , mLogic(logic)
{
    //// 1) подготовим RenderStates, если нужно устанавливать текстуру для тайловой карты
    ////    (примерно так, если вы рендерите вершинный массив)
    //// mStates.texture = &mLogic.getMapTexture();

    //// 2) загрузим шрифт для текста паузы
    //if (!mFont.loadFromFile("assets/fonts/arial.ttf"))
    //    throw std::runtime_error("Failed to load font arial.ttf");

    //// 3) настроим полупрозрачный прямоугольник поверх всего экрана
    ////    размер возьмём из текущего вида
    //sf::Vector2f viewSize = mWindow.getView().getSize();
    //mPauseOverlay.setSize(viewSize);
    //mPauseOverlay.setFillColor(sf::Color(0, 0, 0, 150)); // чёрный с alpha=150

    //// 4) настроим текст
    //mPauseText.setFont(mFont);
    //mPauseText.setString("PAUSED\n\nPress ESC to resume");
    //mPauseText.setCharacterSize(48);
    //mPauseText.setFillColor(sf::Color::White);

    //// центрируем текст по середине вида
    //sf::FloatRect tb = mPauseText.getLocalBounds();
    //mPauseText.setOrigin(tb.left + tb.width / 2.f,
    //    tb.top + tb.height / 2.f);
    //mPauseText.setPosition(viewSize.x / 2.f, viewSize.y / 2.f);
}

void GameRenderer::render()
{
    mWindow.clear();

    // 1) карта
    mLogic.renderScene(mWindow);

    // 2) Если сейчас пауза — накинем оверлей и текст
    if (mLogic.isPaused()) {
        mWindow.draw(mPauseOverlay);
        mWindow.draw(mPauseText);
    }

    mWindow.display();
}
