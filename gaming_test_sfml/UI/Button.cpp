#include "Button.h"
#include <iostream>

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

    // Проверка состояния
    if (contains) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            mState = State::Pressed;
            mWasPressed = true;
        }
        else {
            if (mWasPressed && mOnClick) {
                try {
                    mOnClick();
                }
                catch (const std::exception& e) {
                    std::cerr << "Error in button callback: " << e.what() << std::endl;
                }
            }
            mState = State::Hover;
            mWasPressed = false;
        }
    }
    else {
        mState = State::Normal;
        mWasPressed = false;
    }

    // Установка текстуры по состоянию
    switch (mState) {
    case State::Normal: mSprite.setTexture(mTexN); break;
    case State::Hover:  mSprite.setTexture(mTexH); break;
    case State::Pressed:mSprite.setTexture(mTexP); break;
    }
}

void Button::draw(sf::RenderTarget& target) const
{
    target.draw(mSprite);
    if (mText.getString().getSize() > 0) {
        target.draw(mText);
    }
}

void Button::setText(const std::string& text, const sf::Font& font, unsigned int characterSize) {
    mText.setString(text);
    mText.setFont(font);
    mText.setCharacterSize(characterSize);
    mText.setFillColor(sf::Color::White);
    updateTextPosition();
}

void Button::updateTextPosition() {
    // Центрирование текста на кнопке
    sf::FloatRect textBounds = mText.getLocalBounds();
    sf::FloatRect spriteBounds = mSprite.getGlobalBounds();
    
    float xPos = spriteBounds.left + (spriteBounds.width - textBounds.width) / 2.f;
    float yPos = spriteBounds.top + (spriteBounds.height - textBounds.height) / 2.f;
    
    mText.setPosition(xPos, yPos);
}
