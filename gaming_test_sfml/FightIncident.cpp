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

    mAlertFrameCount = 15;                // для вопросика — 15 кадров!
    mAlertFrameDuration = 0.1f;

     // Генерируем кадры для анимации
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


    // поставить центр вращения спрайта вопроса в его середину
    mAlert.setOrigin(frameSize.x / 2.f, frameSize.y / 2.f);

    // поставим «?» чуть выше крыши
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

    sf::Vector2f pos = mFightSprite.getPosition();
    float spriteW = mFightSprite.getGlobalBounds().width;
    float alertW = mAlert.getGlobalBounds().width;
    float alertH = mAlert.getGlobalBounds().height;

    float shiftX = 17.f; // сдвиг по X 
    float shiftY = 0.f; // сдвиг по Y 

    // Центр по X, чуть выше головы драки

    mAlert.setPosition(
        pos.x + spriteW / 2.f - alertW / 2.f + shiftX,
        pos.y - alertH - 8.f + shiftY
    );
    target.draw(mAlert);


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
