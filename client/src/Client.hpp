#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../includes/nibbler.hpp"

class Client
{
public:
    Client(std::atomic<bool> &stopFlag);
    ~Client();

    void start();
    std::vector<std::string> getGameField() const;

private:
    int serverSocket;
    struct pollfd serverFd;
    char readBuf[16384]; // 16 kb
    std::vector<std::string> gameField;
    std::atomic<bool> &stopFlag;

    void receiveGameData();
    void deserealizeGameData(int bytesRead);
    void printGameField() const;
};

#endif
