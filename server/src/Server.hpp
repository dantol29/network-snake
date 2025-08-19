#ifndef SERVER_HPP
#define SERVER_HPP

#include "Game.hpp"
#include "../includes/nibbler.hpp"
#include <map>

class Server
{
public:
    Server(Game *game);

    void start();

private:
    Game *game;
    int serverFd;
    std::vector<struct pollfd> connectedClients;
    std::vector<int> closedConnections;
    std::unordered_map<int, int> fdToBytesWritten;

    void acceptNewConnection();
    void closeConnection(int index, int fd);
    void removeClosedConnections();
    void sendGameData(int fd);
    void receiveDataFromClient(int fd, int index);
    void handleSocketError(int fd, int index);
};

#endif
