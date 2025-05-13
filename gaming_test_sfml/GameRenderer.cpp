#include "GameRenderer.h"
#include <stdexcept>

GameRenderer::GameRenderer(sf::RenderWindow& window, GameLogic& logic)
    : mWindow(window)
    , mLogic(logic)
{
    //// 1) ���������� RenderStates, ���� ����� ������������� �������� ��� �������� �����
    ////    (�������� ���, ���� �� ��������� ��������� ������)
    //// mStates.texture = &mLogic.getMapTexture();

    //// 2) �������� ����� ��� ������ �����
    //if (!mFont.loadFromFile("assets/fonts/arial.ttf"))
    //    throw std::runtime_error("Failed to load font arial.ttf");

    //// 3) �������� �������������� ������������� ������ ����� ������
    ////    ������ ������ �� �������� ����
    //sf::Vector2f viewSize = mWindow.getView().getSize();
    //mPauseOverlay.setSize(viewSize);
    //mPauseOverlay.setFillColor(sf::Color(0, 0, 0, 150)); // ������ � alpha=150

    //// 4) �������� �����
    //mPauseText.setFont(mFont);
    //mPauseText.setString("PAUSED\n\nPress ESC to resume");
    //mPauseText.setCharacterSize(48);
    //mPauseText.setFillColor(sf::Color::White);

    //// ���������� ����� �� �������� ����
    //sf::FloatRect tb = mPauseText.getLocalBounds();
    //mPauseText.setOrigin(tb.left + tb.width / 2.f,
    //    tb.top + tb.height / 2.f);
    //mPauseText.setPosition(viewSize.x / 2.f, viewSize.y / 2.f);
}

void GameRenderer::render()
{
    mWindow.clear();

    // 1) �����
    mLogic.renderScene(mWindow);

    // 2) ���� ������ ����� � ������� ������� � �����
    if (mLogic.isPaused()) {
        mWindow.draw(mPauseOverlay);
        mWindow.draw(mPauseText);
    }

    mWindow.display();
}
