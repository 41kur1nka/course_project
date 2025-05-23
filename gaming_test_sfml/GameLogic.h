#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Map.h"
#include <vector>
#include <memory>
#include "ParkingViolation.h"
#include "Incident.h"
#include "string"
#include <unordered_map>

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


    // spawning cars
    void setMaxIncidents(std::size_t max) { mMaxIncidents = max; }
    bool spawnOne();

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

    std::unordered_map<std::string, sf::Texture> mCarSideTextures;
    std::unordered_map<std::string, sf::Texture> mCarFrontTextures;
    sf::Texture                                  mQuestionTexture;

    // ��������� ������
    std::size_t mMaxIncidents = 5;          // �� ��������� �� 5
    float       mSpawnInterval = 3.f;        // ��� � 3 �������
    float       mTimeSinceLast = 0.f;        // ���������� �������

    // ���������������
    static std::string pickColor(const std::string& desired,
        const std::unordered_map<std::string, sf::Texture>& dict);
};
