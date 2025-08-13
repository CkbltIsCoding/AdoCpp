#include <iostream>
#include "Game.h"

#ifdef NDEBUG
// NO CMD
#pragma comment(linker, "/subsystem:\"Windows\" /entry:\"mainCRTStartup\"")
#endif // NDEBUG

int main()
{
#ifdef NDEBUG
    try
    {
        Game game;
        game.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
    }
#else
    Game game;
    game.run();
#endif
    return 0;
}
