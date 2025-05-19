#include <SFML/Graphics.hpp>
#include <iostream>
#include "GameLogic.h"
#include "GameRenderer.h"

int main()
{
    try {
        GameLogic logic;
        sf::IntRect playArea = logic.getPlayableArea();

        sf::VideoMode mode(playArea.width, playArea.height);
        sf::RenderWindow window(mode, "Beat Cops", sf::Style::Close);

        GameRenderer renderer(window, logic);
        sf::Clock clock;
        while (window.isOpen()) {
            sf::Time deltaTime = clock.restart();
            sf::Event e;
            while (window.pollEvent(e)) {
                if (e.type == sf::Event::Closed)
                    window.close();
                else if (e.type == sf::Event::KeyPressed)
                    logic.handleInput(e.key.code, true);
                else if (e.type == sf::Event::KeyReleased)
                    logic.handleInput(e.key.code, false);
                else if (e.type == sf::Event::MouseButtonPressed &&
                    e.mouseButton.button == sf::Mouse::Left &&
                    logic.isPaused())         // только в паузе кнопки активны
                {
                    // получаем координаты клика в мире окна
                    sf::Vector2f clickPos = window.mapPixelToCoords(
                        { e.mouseButton.x, e.mouseButton.y });
                    renderer.handleMouseClick(clickPos, window);
                }
                else
                {
                    logic.handleInput(e.key.code, false);
                }
            }

            logic.update(deltaTime);
            renderer.render();

           
        }
        
    }
    catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
