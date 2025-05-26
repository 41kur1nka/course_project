#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

#include "GameLogic.h"
#include "GameRenderer.h"

enum class GameState {
    MainMenu,
    Playing,
    Paused,
    HighScores,
    Settings
};

#include "UI/MainMenuScreen.h"
#include "UI/PauseScreen.h"

class Game {
public:
    Game();
    void run();

private:
    void processInput();
    void update(sf::Time dt);
    void render();

    GameLogic          mLogic;
    sf::RenderWindow   mWindow;
    GameState          mState;
    MainMenuScreen     mMainMenu;
    GameRenderer       mRenderer;
    sf::Clock          mClock;
    PauseScreen mPauseScreen;


};
