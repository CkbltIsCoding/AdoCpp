#include "Game.h"

#ifdef NDEBUG
// NO CMD
#pragma comment(linker,"/subsystem:\"Windows\" /entry:\"mainCRTStartup\"")
#endif // NDEBUG

int main()
{
	Game game;
	game.run();
	return 0;
}
