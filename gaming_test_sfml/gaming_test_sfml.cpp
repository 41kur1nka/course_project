#include "Game.h"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <iostream>

int main()
{
    std::string bin_id = "683c96218a456b7966a822fb";
    std::string api_key = "$2a$10$6vJGt4xm./2k86V8dSn9k.byyRPOExZO9Niqipr3P9cTtPXSRuf5C";
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
