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
        onerror("Invalid args");

    int height = atoi(argv[1]);
    int width = atoi(argv[2]);
    if (height < 5 || width < 5)
        onerror("Invalid size");

    Game *game = new Game(height, width);
    Server *server = new Server(game);
    Snake *snake = new Snake(height, width, game);

    game->addSnake(snake);

    std::thread gameThread(&Game::gameLoop, game);
    std::thread serverThread(&Server::start, server);
    if (gameThread.joinable())
        gameThread.join();
    if (serverThread.joinable())
        serverThread.join();

    delete game;
    delete snake;
    delete server;
}
