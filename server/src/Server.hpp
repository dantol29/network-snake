#ifndef SERVER_HPP
#define SERVER_HPP

#include "../includes/nibbler.hpp"
#include "Game.hpp"
#include <map>

using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;

class Game;

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
    std::string serializedGameData;
    TimePoint lastSendTime;
    char readBuf[10];

    void removeDeadSnakes();
    void acceptNewConnection();
    void closeConnection(int fd);
    void removeClosedConnections();
    void sendGameData(int fd) const;
    void receiveDataFromClient(int fd, int index);
    void handleSocketError(int fd, int index);
    std::string serializeGameData();
};

#endif
