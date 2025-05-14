#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "IncidentManager.h"
#include "Map.h"

class GameLogic {
public:
    GameLogic();

    void handleInput(sf::Keyboard::Key key, bool isPressed);
    void update(sf::Time dt);
    void spawnIncident();

    void togglePause();
    bool isPaused() const;
    void renderScene(sf::RenderWindow& window);

    Player& player() { return mPlayer; }
    IncidentManager& incidents() { return mIncidents; }
    Map& map() { return mMap; }

    sf::Vector2u getMapSize()      const { return mMap.getMapSize(); }
    sf::Vector2u getTileSize()     const { return mMap.getTileSize(); }
    sf::Vector2u getMapPixelSize() const {
        auto ms = mMap.getMapSize();
        auto ts = mMap.getTileSize();
        return { ms.x * ts.x, ms.y * ts.y };
    }
    sf::IntRect getPlayableArea() const {
        // Ќапример, захардкодить или вычислить по данным JSON-карты:
        return sf::IntRect(0, 0,   // смещение
            1280, 960); // ширина и высота области
    }

private:
    Map             mMap;
    Player          mPlayer;
    IncidentManager mIncidents;

    // флаги дл€ WASD
    bool mUp = false, mDown = false, mLeft = false, mRight = false, mIsPaused = false;

    
};
