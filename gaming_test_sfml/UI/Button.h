#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

class Button {
public:
    Button(const sf::Texture& texNormal,
        const sf::Texture& texHover,
        const sf::Texture& texPressed,
        const sf::Vector2f& position,
        std::function<void()> onClick);

    // Checking status (hover, pressed) and onClick 
    void update(const sf::RenderWindow& window);

    // Draw button
    void draw(sf::RenderTarget& target) const;

    // Setting text for button
    void setText(const std::string& text, const sf::Font& font, unsigned int characterSize = 20);

private:
    enum class State { Normal, Hover, Pressed };
    State                   mState = State::Normal;
    sf::Sprite             mSprite;
    const sf::Texture&     mTexN;
    const sf::Texture&     mTexH;
    const sf::Texture&     mTexP;
    std::function<void()>  mOnClick;
    bool                   mWasPressed = false;

    // Button text
    sf::Text              mText;
    void                  updateTextPosition();
};
