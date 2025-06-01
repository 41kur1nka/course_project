#pragma once
#include "Incident.h"
#include <SFML/Graphics.hpp>
#include <vector>

class GraffitiIncident : public Incident {
public:
    GraffitiIncident(const sf::Vector2f& position, const sf::Texture& graffitiTex, const sf::Texture& questionTex,
        unsigned frameCount, sf::Vector2u frameSize, float frameDuration = 0.1f);

    void update(const sf::Vector2f& playerPos, sf::Time dt, const sf::FloatRect& playerBounds) override;
    void render(sf::RenderTarget& target) override;
    bool isPlayerInRange() const { return mPlayerNear; }
    bool isResolved() const override { return mResolved; }
    void resolve() { mResolved = true; }

    sf::FloatRect getGraffitiBounds() const { return mGraffitiSprite.getGlobalBounds(); }

private:
    sf::Sprite mGraffitiSprite;
    sf::Sprite mAlert;
    std::vector<sf::IntRect> mFrames;
    std::vector<sf::IntRect> mAlertFrames;

    unsigned mFrameCount;
    unsigned mCurrentFrame = 0;
    float mFrameDuration;
    float mElapsedTime = 0.f;

    unsigned mAlertFrameCount;
    unsigned mAlertCurrentFrame = 0;
    float mAlertFrameDuration;
    float mAlertElapsedTime = 0.f;

    float mTriggerDist = 32.f;
    bool mPlayerNear = false;
    bool mResolved = false;

    sf::Font mFont;
};
