#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

class Button {
public:
    // texNormal, texHover, texPressed Ч три состо€ни€ кнопки
    // position Ч верхний-левый угол кнопки
    // onClick    Ч вызываетс€ при отпускании Ћ ћ над кнопкой
    Button(const sf::Texture& texNormal,
        const sf::Texture& texHover,
        const sf::Texture& texPressed,
        const sf::Vector2f& position,
        std::function<void()> onClick);

    // ќбновить состо€ние (hover, pressed) и вызвать onClick, если надо
    void update(const sf::RenderWindow& window);

    // Ќарисовать кнопку
    void draw(sf::RenderTarget& target) const;

private:
    enum class State { Normal, Hover, Pressed };
    State                   mState = State::Normal;
    sf::Sprite              mSprite;
    const sf::Texture& mTexN;
    const sf::Texture& mTexH;
    const sf::Texture& mTexP;
    std::function<void()>   mOnClick;
    bool                    mWasPressed = false;
};
