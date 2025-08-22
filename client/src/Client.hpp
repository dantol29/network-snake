#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../includes/nibbler.hpp"

class Client
{
public:
    Client(std::atomic<bool> &stopFlag);
    ~Client();

    void start();
    void setDirection(enum direction dir);

    const std::vector<std::string> &getGameField();
    std::mutex &getGameFieldMutex();
    int getWidth();
    int getHeight();

    static std::string deserealizeValue(char *readBuf, int *index);
    static void printError(std::string str);

private:
    int serverSocket;
    char readBuf[16384]; // 16 kb
    struct pollfd serverFd;
    enum direction previousDirection;

    // Accessed by drawer thread
    std::mutex gameFieldMutex;
    std::vector<std::string> gameField;
    std::atomic<bool> &stopFlag;
    std::atomic<enum direction> direction;
    std::atomic<int> height;
    std::atomic<int> width;

    void receiveGameData();
    void deserealizeGameData(int bytesRead);
    void sendDirection();
    void enableSend(enum direction newDirection);
};

#endif
