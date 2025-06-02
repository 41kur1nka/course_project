#include "Game.h"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <iostream>

int main()
{
    try {
        Game game;
        game.run();
    }
    catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
    
}
