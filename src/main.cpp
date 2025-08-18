#include "../includes/nibbler.hpp"
#include "Game.hpp"
#include "Snake.hpp"

[[noreturn]] void onerror(const char *msg)
{
    write(STDERR_FILENO, msg, strlen(msg));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc != 3)
        onerror("Invalid args");

    int height = atoi(argv[1]);
    int width = atoi(argv[2]);
    if (height == 0 || width == 0)
        onerror("Invalid size");

    Game *game = new Game(height, width);
    Snake *snake = new Snake(height, width, game);

    game->printField();
    game->addSnake(snake);
    game->loadDynamicLibrary("libs/lib1/lib1.dylib");
    game->openWindow();
    game->startGameLoop();

    delete game;
}
