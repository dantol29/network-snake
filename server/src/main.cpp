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
    if (argc != 2)
        onerror("Invalid args");

    int size = atoi(argv[1]);
    if (size < 10)
        onerror("Invalid size");

    Game *game = new Game(size);
    Server *server = new Server(game);

    std::thread serverThread(&Server::start, server);
    
    game->gameLoop();

    if (serverThread.joinable())
        serverThread.join();

    delete game;
    delete server;
}
