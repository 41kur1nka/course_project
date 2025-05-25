#include "Game.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>

Game::Game()
    : mLogic()  // ���������� GameLogic::GameLogic(), �� ��� ��������� map.json
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
    // ����� ����� ��� ��������� ������ ����, ���������������� ������ � �.�.
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
            // TODO: ��������� ������ �� ������� Resume/Exit
            break;

        
        case GameState::HighScores:
        case GameState::Settings:
            // TODO: ������� ����� ��������� ����
            break;
        }
    }
}

void Game::update(sf::Time dt)
{
    if (mState == GameState::MainMenu) {
        // ��������� ������ ������ ����:
        mMainMenu.update(mWindow);
        // � ����� ������� � ������ �������� ��� ���
        return;
    }
    if (mState == GameState::Playing) {
        mLogic.update(dt);
    }
    // ��� ��������� ��������� � ���� ��� ������
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
        // ������� �����
        mRenderer.render();
        // ����� �������������� ���
        sf::RectangleShape overlay(
            sf::Vector2f((float)mWindow.getSize().x,
                (float)mWindow.getSize().y)
        );
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        mWindow.draw(overlay);
        // TODO: ���������� ������ Resume/Exit
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
