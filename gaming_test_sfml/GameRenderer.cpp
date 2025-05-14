#include "GameRenderer.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>

GameRenderer::GameRenderer(sf::RenderWindow& window, GameLogic& logic)
    : mWindow(window)
    , mLogic(logic)
{
    // 2) �������� ����� ��� ������ �����
    if (!mFont.loadFromFile("assets/fonts/Montserrat-Black.ttf"))
        throw std::runtime_error("Failed to load font arial.ttf");

    //// 3) �������� �������������� ������������� ������ ����� ������
    ////    ������ ������ �� �������� ����
    sf::Vector2f viewSize = mWindow.getView().getSize();
    mPauseOverlay.setSize(viewSize);
    mPauseOverlay.setFillColor(sf::Color(0, 0, 0, 150)); // ������ � alpha=150

    // 4) �������� �����
    mPauseText.setFont(mFont);
    mPauseText.setString("PAUSED");
    mPauseText.setCharacterSize(48);
    mPauseText.setFillColor(sf::Color::White);

    // ���������� ����� �� �������� ����
    sf::FloatRect tb = mPauseText.getLocalBounds();
    mPauseText.setOrigin(tb.left + tb.width / 2.f,
        tb.top + tb.height / 2.f);
    mPauseText.setPosition(viewSize.x / 2.f, viewSize.y / 2.f);

    //Resume button
    mResumeBtn.setSize({ 200.f,60.f });
    mResumeBtn.setFillColor(sf::Color(100, 100, 100));
    mResumeBtn.setPosition(viewSize.x / 2.f - 100.f,viewSize.y / 2.f - 40.f);
    
    mResumeText.setFont(mFont);
    mResumeText.setString("Resume");
    mPauseText.setCharacterSize(48);
    mPauseText.setFillColor(sf::Color::White);
    mResumeText.setPosition(
        mResumeBtn.getPosition().x + (mResumeBtn.getSize().x - mResumeText.getLocalBounds().width) / 2.f,
        mResumeBtn.getPosition().y + (mResumeBtn.getSize().y - mResumeText.getLocalBounds().height) / 2.f - 5.f
    );

    //Exit button
    mExitBtn = mResumeBtn;
    mExitBtn.setPosition(viewSize.x / 2.f - 100.f, viewSize.y / 2.f + 40.f);
    mExitText = mResumeText;
    mExitText.setString("Exit");
    mExitText.setPosition(mExitBtn.getPosition().x + (mExitBtn.getSize().x - mExitText.getLocalBounds().width) / 2.f,
        mExitBtn.getPosition().y + (mExitBtn.getSize().y - mExitText.getLocalBounds().height) / 2.f - 5.f
    );
}

void GameRenderer::render()
{
    // 1) �������� �������������
    sf::IntRect playArea = mLogic.getPlayableArea();

    // 2) ����������� View
    sf::View view;
    view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));      // �� ���� �����
    view.setSize(static_cast<float>(playArea.width),
        static_cast<float>(playArea.height));
    view.setCenter(
        playArea.left + playArea.width * 0.5f,
        playArea.top + playArea.height * 0.5f
    );
    float scaleFactor = 1.f;
    view.zoom(1.f / scaleFactor);

    mWindow.setView(view);

    // 3) ������� � ������
    mWindow.clear();
    mLogic.renderScene(mWindow);

    // 4) ���������� ��� ��� UI/����
    mWindow.setView(mWindow.getDefaultView());
    if (mLogic.isPaused()) {
        /*mWindow.draw(mPauseOverlay);
        mWindow.draw(mPauseText);*/

        //Draw buttons
        mWindow.draw(mResumeBtn);
        mWindow.draw(mResumeText);
        mWindow.draw(mExitBtn);
        mWindow.draw(mExitText);
    }

    mWindow.display();
}

void GameRenderer::handleMouseClick(const sf::Vector2f& mousePos, sf::RenderWindow& window)
{
    // �������� �� Resume?
    if (mResumeBtn.getGlobalBounds().contains(mousePos)) {
        mLogic.togglePause();    // ����� �����
        return;
    }
    // �������� �� Exit?
    if (mExitBtn.getGlobalBounds().contains(mousePos)) {
        window.close();          // ����� �� ���������
    }
}
