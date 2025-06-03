#include "Game.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>

Game::Game()
    : mLogic(std::make_unique<GameLogic>())
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
    , mSettingsScreen(mState, mSettings)

{
    mMainMenu.loadAssets(mWindow);
    mPauseScreen->loadAssets(mWindow);
    mScoresScreen.loadAssets(mWindow);
    mSettings.load();
    mSettingsScreen.loadAssets(mWindow);
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
                
                int playerScore = mLogic->getScore();

                auto trim = [](std::string s) {
                    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
                    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
                    return s;
                    };
                std::string safeName = trim(mSettings.name);
                if (safeName.empty()) safeName = "Player";
                std::cout << "Sending score for: " << mSettings.name << " value: " << playerScore << std::endl;
                mScoresManager.sendScoreOnline(mSettings.name, playerScore);
            }
            break;

        
        case GameState::HighScores:
            break;

        case GameState::Settings:
            mSettingsScreen.update(mWindow, e);
            break;
        }
    }
}

void Game::update(sf::Time dt)
{
    static bool wasInScores = false;
    if (mState == GameState::MainMenu ) {
        
        mMainMenu.update(mWindow);
        
        return;
    }
    if (mState == GameState::Playing) {
        mLogic->getPlayer().setSkin(mSettings.skinIndex);
        mLogic->update(dt);
    }
    if (mState == GameState::Paused) {
        mPauseScreen->update(mWindow);
        return;
    }
    if (mState == GameState::HighScores) {
        
        if (!wasInScores) {
            mScoresScreen.loadScoresOnline();
            wasInScores = true;
        }
        mScoresScreen.update(mWindow);
        return;
    }
    else { wasInScores = false; }
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

    case GameState::Settings:
        mSettingsScreen.draw(mWindow);
        mWindow.display();
        break;

    default:
        break;
    }
}
