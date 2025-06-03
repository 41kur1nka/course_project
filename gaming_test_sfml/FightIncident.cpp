#include "FightIncident.h"
#include <cmath>
#include <iostream>

FightIncident::FightIncident(const sf::Vector2f& position, const sf::Texture& fightTex, const sf::Texture& questionTex,
    unsigned frameCount, sf::Vector2u frameSize, float frameDuration)
    : mFrameCount(frameCount), mFrameDuration(frameDuration)
{
    mFightSprite.setTexture(fightTex);
    mFightSprite.setPosition(position);

    mAlert.setTexture(questionTex);

    mAlertFrameCount = 15;
    mAlertFrameDuration = 0.1f;

    for (unsigned i = 0; i < mFrameCount; ++i) {
        mFrames.emplace_back(
            i * frameSize.x, 0,
            frameSize.x, frameSize.y
        );
    }
    for (unsigned i = 0; i < mAlertFrameCount; ++i) {
        mAlertFrames.emplace_back(
            i * 6, 0, 6, 23
        );
    }
    mFightSprite.setTextureRect(mFrames[0]);
    mAlert.setTextureRect(mAlertFrames[0]);


    mAlert.setOrigin(frameSize.x / 2.f, frameSize.y / 2.f);

    mAlert.setScale(1.5f, 1.5f);

    if (!mFont.loadFromFile("assets/fonts/Montserrat-Black.ttf")) {
        std::cout << "cant load font!" << std::endl;
    }
}

void FightIncident::update(const sf::Vector2f& playerPos, sf::Time dt, const sf::FloatRect& playerBounds)
{
    mElapsedTime += dt.asSeconds();
    if (mElapsedTime >= mFrameDuration) {
        mCurrentFrame = (mCurrentFrame + 1) % mFrameCount;
        mFightSprite.setTextureRect(mFrames[mCurrentFrame]);
        mElapsedTime = 0.f;
    }

    mAlertElapsedTime += dt.asSeconds();
    if (mAlertElapsedTime >= mAlertFrameDuration) {
        mAlertCurrentFrame = (mAlertCurrentFrame + 1) % mAlertFrameCount;
        mAlert.setTextureRect(mAlertFrames[mAlertCurrentFrame]);
        mAlertElapsedTime = 0.f;
    }

    sf::FloatRect fightRect = mFightSprite.getGlobalBounds();

    float buffer = 8.f;
    fightRect.left -= buffer;
    fightRect.top -= buffer;
    fightRect.width += 2 * buffer;
    fightRect.height += 2 * buffer;

    mPlayerNear = fightRect.intersects(playerBounds);

    
}

void FightIncident::render(sf::RenderTarget& target)
{
    target.draw(mFightSprite);

    sf::Vector2f pos = mFightSprite.getPosition();
    float spriteW = mFightSprite.getGlobalBounds().width;
    float alertW = mAlert.getGlobalBounds().width;
    float alertH = mAlert.getGlobalBounds().height;

    float shiftX = 17.f;
    float shiftY = 0.f;

    mAlert.setPosition(
        pos.x + spriteW / 2.f - alertW / 2.f + shiftX,
        pos.y - alertH - 8.f + shiftY
    );
    target.draw(mAlert);


    // hint "Press E"
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
