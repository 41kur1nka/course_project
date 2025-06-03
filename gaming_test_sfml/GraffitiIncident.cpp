#include "GraffitiIncident.h"
#include <cmath>
#include <iostream>

GraffitiIncident::GraffitiIncident(const sf::Vector2f& position, const sf::Texture& graffitiTex, const sf::Texture& questionTex,
    unsigned frameCount, sf::Vector2u frameSize, float frameDuration)
    : mFrameCount(frameCount), mFrameDuration(frameDuration)
{



    mGraffitiSprite.setTexture(graffitiTex);
    mGraffitiSprite.setPosition(position);

    for (unsigned i = 0; i < mFrameCount; ++i) {
        mFrames.emplace_back(
            i * frameSize.x, 0,
            frameSize.x, frameSize.y
        );
    }
    mGraffitiSprite.setTextureRect(mFrames[0]);

    mAlert.setTexture(questionTex);
    mAlertFrameCount = 15;
    mAlertFrameDuration = 0.1f;
    for (unsigned i = 0; i < mAlertFrameCount; ++i) {
        mAlertFrames.emplace_back(
            i * 6, 0, 6, 23
        );
    }
    mAlert.setTextureRect(mAlertFrames[0]);
    mAlert.setOrigin(6 / 2.f, 23 / 2.f);
    mAlert.setScale(1.5f, 1.5f);

    if (!mFont.loadFromFile("assets/fonts/Montserrat-Black.ttf")) {
        std::cout << "cant load font!" << std::endl;
    }


}

void GraffitiIncident::update(const sf::Vector2f& playerPos, sf::Time dt, const sf::FloatRect& playerBounds)
{
    // Graffiti animation
    mElapsedTime += dt.asSeconds();
    if (mElapsedTime >= mFrameDuration) {
        mCurrentFrame = (mCurrentFrame + 1) % mFrameCount;
        mGraffitiSprite.setTextureRect(mFrames[mCurrentFrame]);
        mElapsedTime = 0.f;
    }

    // Sign animation
    mAlertElapsedTime += dt.asSeconds();
    if (mAlertElapsedTime >= mAlertFrameDuration) {
        mAlertCurrentFrame = (mAlertCurrentFrame + 1) % mAlertFrameCount;
        mAlert.setTextureRect(mAlertFrames[mAlertCurrentFrame]);
        mAlertElapsedTime = 0.f;
    }

    // Checking crossing
    sf::FloatRect graffitiRect = mGraffitiSprite.getGlobalBounds();
    float buffer = 8.f;
    graffitiRect.left -= buffer;
    graffitiRect.top -= buffer;
    graffitiRect.width += 2 * buffer;
    graffitiRect.height += 2 * buffer;
    mPlayerNear = graffitiRect.intersects(playerBounds);
}

void GraffitiIncident::render(sf::RenderTarget& target)
{
    target.draw(mGraffitiSprite);

    sf::Vector2f pos = mGraffitiSprite.getPosition();
    float spriteW = mGraffitiSprite.getGlobalBounds().width;
    float alertW = mAlert.getGlobalBounds().width;
    float alertH = mAlert.getGlobalBounds().height;

    float shiftX = 5.f; 
    float shiftY = 20.f;

    mAlert.setPosition(
        pos.x + spriteW / 2.f - alertW / 2.f + shiftX,
        pos.y - alertH - 8.f + shiftY
    );
    target.draw(mAlert);

    if (mPlayerNear && !mResolved) {
        sf::Text pressE;
        pressE.setFont(mFont);
        pressE.setString("Press E for interacting");
        pressE.setCharacterSize(20);
        pressE.setFillColor(sf::Color::White);
        pressE.setOutlineColor(sf::Color::Black);
        pressE.setOutlineThickness(2.f);
        float x = 1280 - 350;
        float y = 960 - 60;
        pressE.setPosition(x, y);
        target.draw(pressE);
    }
}
