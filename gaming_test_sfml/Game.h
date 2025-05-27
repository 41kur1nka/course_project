#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

#include "GameLogic.h"
#include "GameRenderer.h"
#include "HighScoresManager.h"

enum class GameState {
    MainMenu,
    Playing,
    Paused,
    HighScores,
    Settings
};

#include "UI/MainMenuScreen.h"
#include "UI/PauseScreen.h"
#include "UI/HighScoresScreen.h"

class Game {
public:
    Game();
    void run();
    void resetLogic();
private:
    void processInput();
    void update(sf::Time dt);
    void render();

    std::unique_ptr<GameLogic>          mLogic;
    sf::RenderWindow   mWindow;
    GameState          mState;
    MainMenuScreen     mMainMenu;
    std::unique_ptr<GameRenderer> mRenderer;
    sf::Clock          mClock;

    //Pause screen
    std::unique_ptr<PauseScreen> mPauseScreen;

    //Score screen
    HighScoresManager mScoresManager;
    HighScoresScreen  mScoresScreen;

};
