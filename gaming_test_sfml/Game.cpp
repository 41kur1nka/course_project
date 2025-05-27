#include "Game.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>

Game::Game()
    : mLogic(std::make_unique<GameLogic>())  // используем GameLogic::GameLogic(), он сам загружает map.json
    , mWindow(
        sf::VideoMode(
            mLogic->getMapSize().x* mLogic->getTileSize().x,
            mLogic->getMapSize().y* mLogic->getTileSize().y
        ),
        "Beat Cops",
        sf::Style::Close
    )
    , mState(GameState::MainMenu) 
    , mMainMenu(mState, *this)
    , mRenderer(std::make_unique<GameRenderer>(mWindow, *mLogic))
    , mPauseScreen(std::make_unique<PauseScreen>(mState, mScoresManager, *mLogic))
    , mScoresManager("scores.txt")
    , mScoresScreen(mState, mScoresManager)

{
    // здесь можно ещЄ настроить иконку окна, инициализировать рандом и т.п.
    mMainMenu.loadAssets(mWindow);
    mPauseScreen->loadAssets(mWindow);
    mScoresScreen.loadAssets(mWindow);
    std::srand(unsigned(std::time(nullptr)));
}

void Game::run()
{
    while (mWindow.isOpen()) {
        processInput();
        sf::Time dt = mClock.restart();
        update(dt);
        render();
    }
}

void Game::resetLogic() {
    mLogic = std::make_unique<GameLogic>();
    mRenderer = std::make_unique<GameRenderer>(mWindow, *mLogic);
    mPauseScreen = std::make_unique<PauseScreen>(mState, mScoresManager, *mLogic);
    mPauseScreen->loadAssets(mWindow);
}


void Game::processInput()
{
    sf::Event e;
    while (mWindow.pollEvent(e)) {
        if (e.type == sf::Event::Closed) {
            mWindow.close();
            return;
        }

        switch (mState) {
        case GameState::Playing:
            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape) {
                mState = GameState::Paused;
            }
            else if (e.type == sf::Event::KeyPressed) {
                mLogic->handleInput(e.key.code, true);
            }
            else if (e.type == sf::Event::KeyReleased) {
                mLogic->handleInput(e.key.code, false);
            }
            break;

        case GameState::Paused:
            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape)
                mState = GameState::Playing;
            if (e.type == sf::Event::MouseButtonReleased &&
                e.mouseButton.button == sf::Mouse::Left)
            {
                mPauseScreen->update(mWindow);
            }
            break;

        
        case GameState::HighScores:
            if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left)
                mScoresScreen.update(mWindow);
            break;

        case GameState::Settings:
            // TODO: добавим позже обработку меню
            break;
        }
    }
}

void Game::update(sf::Time dt)
{
    if (mState == GameState::MainMenu ) {
        // ќбновл€ем кнопки каждый кадр:
        mMainMenu.update(mWindow);
        // и сразу выходим Ч логики геймпле€ тут нет
        return;
    }
    if (mState == GameState::Playing) {
        mLogic->update(dt);
    }
    if (mState == GameState::Paused) {
        mPauseScreen->update(mWindow);
        return;
    }
    if (mState == GameState::HighScores) {
        mScoresScreen.update(mWindow);
        return;
    }
    // дл€ остальных состо€ний Ч пока без логики
}

void Game::render()
{
    mWindow.clear();

    switch (mState) {
    case GameState::Playing:
        mRenderer->render();
        mWindow.display();
        break;

    case GameState::Paused: {
        mRenderer->render();
        sf::RectangleShape overlay(
            sf::Vector2f((float)mWindow.getSize().x,
                (float)mWindow.getSize().y)
        );
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        mWindow.draw(overlay);

        mPauseScreen->draw(mWindow);
        mWindow.display();
        break;
    }

    case GameState::MainMenu:
        mMainMenu.draw(mWindow);
        mWindow.display();
        break;

    case GameState::HighScores:
        mScoresScreen.draw(mWindow);
        mWindow.display();
        break;
        //TODO : settings
    default:
        break;
    }
}
