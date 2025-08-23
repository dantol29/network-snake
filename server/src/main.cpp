#include "../includes/nibbler.hpp"
#include "Game.hpp"
#include "Server.hpp"

[[noreturn]] void onerror(const char *msg)
{
    write(STDERR_FILENO, msg, strlen(msg));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc != 3)
        onerror("Usage: ./nibbler height width");

    int height = atoi(argv[1]);
    int width = atoi(argv[2]);
    if (height < 10 || width < 10 || height > 100 || width > 100)
        onerror("Invalid size");

    Game *game = new Game(height, width);
    Server *server = new Server(game);

    std::thread gameThread(&Game::start, game);

    server->start();

    if (gameThread.joinable())
        gameThread.join();

    delete game;
    delete server;
}
