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

Client::~Client()
{
    close(this->serverSocket);
}

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

// TLV format
void Client::deserealizeGameData(int bytesRead)
{
    int index = 0;

    const std::string heightStr = Client::deserealizeValue(readBuf, &index);
    const std::string widthStr = Client::deserealizeValue(readBuf + index, &index);
    const std::string fieldStr = Client::deserealizeValue(readBuf + index, &index);
    if (heightStr.empty() || widthStr.empty() || fieldStr.empty())
        return Client::printError("Could not deserealize incoming data");

    const int height = atoi(heightStr.c_str());
    const int width = atoi(widthStr.c_str());
    if (height == 0 || width == 0)
        return Client::printError("Invalid length or width");

    if (height * width != fieldStr.size())
        return Client::printError("Game field was not received correctly");

    std::lock_guard<std::mutex> lock(this->gameFieldMutex);

    this->gameField.clear();

    std::string row;
    for (int y = 0; y < height; y++)
    {
        row.clear();
        for (int x = 0; x < width; x++)
            row += fieldStr[x + y * height];

        this->gameField.push_back(row);
    }

    this->height.store(height);
    this->width.store(width);
}

std::string Client::deserealizeValue(char *readBuf, int *index)
{
    const int lenSize = readBuf[0] - '0';
    if (lenSize < 1)
        return "";

    const std::string len(readBuf + 1, lenSize);
    const int lenInt = atoi(len.c_str());
    if (lenInt == 0)
        return "";

    *index += 1 + len.size() + lenInt;

    const std::string value(readBuf + 1 + len.size(), lenInt);
    return value;
}

void Client::printError(std::string str)
{
    std::cerr << str << std::endl;
}

const std::vector<std::string> &Client::getGameField()
{
    return this->gameField;
}

std::mutex &Client::getGameFieldMutex()
{
    return this->gameFieldMutex;
}

int Client::getWidth()
{
    return this->width.load();
}

int Client::getHeight()
{
    return this->height.load();
}
