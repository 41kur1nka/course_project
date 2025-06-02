#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

class Button {
public:
    // texNormal, texHover, texPressed - текстуры кнопки
    // position - позиция-левый верх кнопки
    // onClick    - выполняется при нажатии на кнопку
    Button(const sf::Texture& texNormal,
        const sf::Texture& texHover,
        const sf::Texture& texPressed,
        const sf::Vector2f& position,
        std::function<void()> onClick);

 /*   ~Button() {
        mOnClick = nullptr;
    }*/

    // Проверка состояния (hover, pressed) и вызов onClick, если надо
    void update(const sf::RenderWindow& window);

    // Отрисовка кнопки
    void draw(sf::RenderTarget& target) const;

    // Установка текста кнопки
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

    // Текст кнопки
    sf::Text              mText;
    void                  updateTextPosition();
};
