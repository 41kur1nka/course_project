#include "Game.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>

Game::Game()
    : mLogic()  // используем GameLogic::GameLogic(), он сам загружает map.json
    , mWindow(
        sf::VideoMode(
            mLogic.getMapSize().x* mLogic.getTileSize().x,
            mLogic.getMapSize().y* mLogic.getTileSize().y
        ),
        "Beat Cops",
        sf::Style::Close
    )
    , mState(GameState::MainMenu) 
    , mMainMenu(mState)
    , mRenderer(mWindow, mLogic)
{
    // здесь можно ещЄ настроить иконку окна, инициализировать рандом и т.п.
    mMainMenu.loadAssets(mWindow);
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
                mLogic.handleInput(e.key.code, true);
            }
            else if (e.type == sf::Event::KeyReleased) {
                mLogic.handleInput(e.key.code, false);
            }
            break;

        case GameState::Paused:
            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape) {
                mState = GameState::Playing;
            }
            // TODO: обработка кликов по кнопкам Resume/Exit
            break;

        
        case GameState::HighScores:
        case GameState::Settings:
            // TODO: добавим позже обработку меню
            break;
        }
    }
}

void Game::update(sf::Time dt)
{
    if (mState == GameState::MainMenu) {
        // ќбновл€ем кнопки каждый кадр:
        mMainMenu.update(mWindow);
        // и сразу выходим Ч логики геймпле€ тут нет
        return;
    }
    if (mState == GameState::Playing) {
        mLogic.update(dt);
    }
    // дл€ остальных состо€ний Ч пока без логики
}

void Game::render()
{
    mWindow.clear();

    switch (mState) {
    case GameState::Playing:
        mRenderer.render();
        mWindow.display();
        break;

    case GameState::Paused: {
        // сначала сцена
        mRenderer.render();
        // затем полупрозрачный фон
        sf::RectangleShape overlay(
            sf::Vector2f((float)mWindow.getSize().x,
                (float)mWindow.getSize().y)
        );
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        mWindow.draw(overlay);
        // TODO: отрисовать кнопки Resume/Exit
        mWindow.display();
        break;
    }

    case GameState::MainMenu:
        mMainMenu.draw(mWindow);
        mWindow.display();
        break;

        //TODO : highscores, settings
    default:
        break;
    }
}
