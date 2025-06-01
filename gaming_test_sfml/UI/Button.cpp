#include "Button.h"

Button::Button(const sf::Texture& texNormal,
    const sf::Texture& texHover,
    const sf::Texture& texPressed,
    const sf::Vector2f& position,
    std::function<void()> onClick)
    : mTexN(texNormal)
    , mTexH(texHover)
    , mTexP(texPressed)
    , mOnClick(onClick)
{
    mSprite.setTexture(mTexN);
    mSprite.setPosition(position);
}

void Button::update(const sf::RenderWindow& window)
{
    sf::Vector2i mouse = sf::Mouse::getPosition(window);
    bool contains = mSprite.getGlobalBounds().contains((sf::Vector2f)mouse);


    // Выбираем состояние
    if (contains) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            mState = State::Pressed;
            mWasPressed = true;
        }
        else {
            if (mWasPressed) {
                if (mOnClick) mOnClick();
            }
            mState = State::Hover;
            mWasPressed = false;
        }
    }
    else {
        mState = State::Normal;
        mWasPressed = false;
    }

    // Применяем текстуру по состоянию
    switch (mState) {
    case State::Normal: mSprite.setTexture(mTexN); break;
    case State::Hover:  mSprite.setTexture(mTexH); break;
    case State::Pressed:mSprite.setTexture(mTexP); break;
    }
}

void Button::draw(sf::RenderTarget& target) const
{
    target.draw(mSprite);
}
