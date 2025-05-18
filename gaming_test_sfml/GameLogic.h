#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Map.h"
#include <vector>
#include <memory>
#include "ParkingViolation.h"
#include "Incident.h"

class GameLogic {
public:
    GameLogic();

    void handleInput(sf::Keyboard::Key key, bool isPressed);
    void update(sf::Time dt);
    void renderScene(sf::RenderWindow& window);

    bool isPaused() const { return mPaused; }
    bool isInteracting() const { return mInInteraction; }
    void togglePause() { mPaused = !mPaused; }
    sf::Vector2f getPlayerPosition() const { return mPlayer.getPosition(); }

    Player& player() { return mPlayer; }
    Map& map() { return mMap; }

    sf::Vector2u getMapSize()      const { return mMap.getMapSize(); }
    sf::Vector2u getTileSize()     const { return mMap.getTileSize(); }
    sf::Vector2u getMapPixelSize() const {
        auto ms = mMap.getMapSize();
        auto ts = mMap.getTileSize();
        return { ms.x * ts.x, ms.y * ts.y };
    }
    sf::IntRect getPlayableArea() const {
        // ��������, ������������ ��� ��������� �� ������ JSON-�����:
        return sf::IntRect(0, 0,   // ��������
            1280, 960); // ������ � ������ �������
    }
    void startInteraction();
    void finishInteraction(bool ok);

private:
    
    Map             mMap;
    Player          mPlayer;

    std::vector<std::unique_ptr<Incident>> mIncidents;
    ParkingViolation* mCurrentViolation = nullptr;
    bool                mPaused = false;
    bool                mInInteraction = false;
    int                 mScore = 0;

    // �������� ��� ����������
    sf::Texture mCarTexture;
    sf::Texture mQuestionSheetTexture;
};
