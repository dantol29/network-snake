#include "Client.hpp"

#define SERVER_PORT 3000
#define SERVER_IP "127.0.0.1"
#define BLOCKING -1

Client::Client(std::atomic<bool> &stopFlag) : stopFlag(stopFlag), previousDirection(UP), direction(UP)
{
    this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->serverSocket < 0)
        onerror("Socket init error");

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (connect(this->serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        onerror("Connect to server error");

    this->serverFd.fd = this->serverSocket;
    this->serverFd.events = POLLIN;
    this->serverFd.revents = 0;
}

Client::~Client() {}

void Client::start()
{
    while (poll(&this->serverFd, 1, BLOCKING))
    {
        if (this->serverFd.revents & POLLIN)
            receiveGameData();
        if (this->serverFd.revents & POLLOUT)
            sendDirection();

        auto currentDirection = this->direction.load();
        if (this->previousDirection != currentDirection)
            enableSend(currentDirection);

        if (stopFlag.load())
            return;
    }
    onerror("poll error");
}

void Client::receiveGameData()
{
    int bytesRead = read(this->serverSocket, &this->readBuf, 16384);
    if (bytesRead > 0)
        deserealizeGameData(bytesRead);
    else if (bytesRead == 0)
        onerror("Server closed connection");
    else
        onerror("Error while reading from server socket");
}

// TLV format
void Client::deserealizeGameData(int bytesRead)
{
    std::string row;

    const int lenSize = this->readBuf[0] - '0';
    if (lenSize < 1)
        return;

    const std::string dataLen(this->readBuf + 1, lenSize);
    const int dataLenInt = atoi(dataLen.c_str());
    const int rowSize = sqrt(dataLenInt);
    if (dataLenInt == 0 || rowSize == 0)
        return;

    if (1 + lenSize + dataLenInt > bytesRead)
        onerror("Not enough data received");

    std::lock_guard<std::mutex> lock(this->gameFieldMutex);

    this->gameField.clear();

    for (int y = 0; y < rowSize; y++)
    {
        row.clear();
        for (int x = 0; x < rowSize; x++)
            row += this->readBuf[x + lenSize + 1 + y * rowSize];

        this->gameField.push_back(row);
    }

    // this->printGameField();
}

void Client::sendDirection()
{
    char writeBuf[2];
    writeBuf[0] = this->previousDirection;
    writeBuf[1] = '\0';

    int bytesWritten = write(this->serverSocket, &writeBuf, 2);
    if (bytesWritten == 2)
        this->serverFd.events = POLLIN; // disable POLLOUT
    else
        perror("write");
}

void Client::setDirection(enum direction dir)
{
    this->direction.store(dir);
}

void Client::enableSend(enum direction newDirection)
{
    this->previousDirection = newDirection;
    this->serverFd.events = POLLIN | POLLOUT; // enable POLLOUT to send new direction
}

const std::vector<std::string> &Client::getGameField()
{
    return this->gameField;
}

std::mutex &Client::getGameFieldMutex()
{
    return this->gameFieldMutex;
}

void Client::printGameField() const
{
    printf("\n\n");
    for (int i = 0; i < this->gameField.size(); i++)
        printf("%3d:%s\n", i, this->gameField[i].c_str());
    printf("\n\n");
}
