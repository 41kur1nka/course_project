#include <SFML/Graphics.hpp>
#include <iostream>
#include "GameLogic.h"
#include "GameRenderer.h"

int main()
{
    try {
        GameLogic    logic;

        /*sf::Vector2u mapPx = logic.getMapPixelSize();*/

        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        sf::RenderWindow window{ desktop,"Beat Cop Prototype", sf::Style::Fullscreen};

        const float VIRT_W = 1920.f, VIRT_H = 1080.f;
        sf::VideoMode dm = sf::VideoMode::getDesktopMode();
        window.create(dm, "Beat Cop", sf::Style::Fullscreen);

        // задаём «виртуальную» камеру 1920×1080
        sf::View view(sf::FloatRect(0, 0, VIRT_W, VIRT_H));
        window.setView(view);

        /*sf::View view;
        view.setSize(float(mapPx.x), float(mapPx.y));
        view.setCenter(float(mapPx.x) / 2.f, float(mapPx.y) / 2.f);
        window.setView(view);*/
      
        GameRenderer renderer(window, logic);

        sf::Clock clock;
        while (window.isOpen()) {
            sf::Time deltaTime = clock.restart();
            sf::Event e;
            while (window.pollEvent(e)) {
                if (e.type == sf::Event::Closed)
                    window.close();
                if (e.type == sf::Event::KeyPressed)
                    logic.handleInput(e.key.code, true);
                if (e.type == sf::Event::KeyReleased)
                    logic.handleInput(e.key.code, false);
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
