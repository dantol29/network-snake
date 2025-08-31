#ifndef SERVER_HPP
#define SERVER_HPP

#include "../includes/nibbler.hpp"
#include "Game.hpp"

class Game;

class Server
{
public:
    Server(Game *game);
    ~Server();

    void start();

    static std::string serializeValue(const std::string &value);

private:
    Game *game;
    int tcpServerFd;
    int udpServerFd;
    std::vector<struct pollfd> connectedClients;
    std::vector<int> closedConnections;
    char readBuf[10];

    std::string serializedGameField;
    std::string serializedHeight;
    std::string serializedWidth;

    void setupSocket();
    void acceptNewConnection();
    void closeConnection(const int fd);
    void removeClosedConnections();
    void sendGameData(const int fd) const;
    void receiveDataFromClient(const int fd, const int index);
    void handleSocketError(const int fd, const int index);
    std::string serializeGameField();
};

#endif
