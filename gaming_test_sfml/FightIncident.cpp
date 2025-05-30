#include "FightIncident.h"
#include <cmath>
#include <iostream>

FightIncident::FightIncident(const sf::Vector2f& position, const sf::Texture& fightTex,
    unsigned frameCount, sf::Vector2u frameSize, float frameDuration)
    : mFrameCount(frameCount), mFrameDuration(frameDuration)
{
    mFightSprite.setTexture(fightTex);
    mFightSprite.setPosition(position);

    // Генерируем кадры для анимации
    for (unsigned i = 0; i < frameCount; ++i) {
        mFrames.emplace_back(
            i * frameSize.x, 0, frameSize.x, frameSize.y
        );
    }
    mFightSprite.setTextureRect(mFrames[0]);

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

    // Проверка пересечения (аналогично ParkingViolation)
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

    // подсказка "Нажмите E"
    if (mPlayerNear && !mResolved) { // Только если игрок рядом и не решено!
        sf::Text pressE;
        pressE.setFont(mFont);
        pressE.setString("Press E for interacting");
        pressE.setCharacterSize(20);
        pressE.setFillColor(sf::Color::White);
        pressE.setOutlineColor(sf::Color::Black);
        pressE.setOutlineThickness(2.f);
        // Координаты — над машиной или над знаком "?"
        float x = 1280 - 350;
        float y = 960 - 60;
        pressE.setPosition(x, y);
        target.draw(pressE);
    }



}
