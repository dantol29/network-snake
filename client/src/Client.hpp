#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../includes/nibbler.hpp"

class Client
{
public:
    Client(std::atomic<bool> &stopFlag);
    ~Client();

    void start();
    void setDirection(const enum direction dir);

    const std::vector<std::string> &getGameField() const;
    std::mutex &getGameFieldMutex();
    int getWidth() const;
    int getHeight() const;

    static std::string deserealizeValue(const char *readBuf, int *index);
    static void printError(const std::string &str);

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
    void deserealizeGameData(const int bytesRead);
    void sendDirection();
    void enableSend(const enum direction newDirection);
};

#endif
