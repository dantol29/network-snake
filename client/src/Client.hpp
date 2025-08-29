#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../includes/nibbler.hpp"

class Client
{
public:
    Client(std::atomic<bool> &stopFlag);
    ~Client();

    void start();
    void sendDirection(const enum direction newDirection) const;

    const std::vector<std::string> &getGameField() const;
    std::mutex &getGameFieldMutex();
    int getWidth() const;
    int getHeight() const;
    int getSnakeX() const;
    int getSnakeY() const;

    static std::string deserealizeValue(const char *readBuf, int *index);
    static void printError(const std::string &str);

private:
    int tcpSocket;
    int udpSocket;
    sockaddr_in serverAddr;
    char readBuf[16384]; // 16 kb
    std::string buffer;
    struct pollfd serverFd;

    // Accessed by drawer thread
    std::mutex gameFieldMutex;
    std::vector<std::string> gameField;
    std::atomic<bool> &stopFlag;
    std::atomic<int> height;
    std::atomic<int> width;
    std::atomic<int> snakeX;
    std::atomic<int> snakeY;

    void setupSocket();
    void receiveGameData();
    void deserealizeGameData(const int bytesRead);
    void parseGameData(const char *message);
    void updateGameState(int snakeX, int snakeY, int height, int width, const std::string &fieldStr);
};

#endif
