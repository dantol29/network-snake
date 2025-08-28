#include "Client.hpp"

#define SERVER_PORT 45413
#define SERVER_IP "34.1.194.123"
#define BLOCKING -1

Client::Client(std::atomic<bool> &stopFlag) : stopFlag(stopFlag), previousDirection(UP), direction(UP), snakeX(0), snakeY(0), height(0), width(0)
{
    this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->serverSocket < 0)
        onerror("Socket init error");

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    std::cout << "connecting..." << std::endl;

    if (connect(this->serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        onerror("Connect to server error");

    std::cout << "connected" << std::endl;

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

// TODO: might be a better way to enable POLLOUT without check in a loop
void Client::enableSend(const enum direction newDirection)
{
    this->previousDirection = newDirection;
    this->serverFd.events = POLLIN | POLLOUT; // enable POLLOUT to send new direction
}

void Client::deserealizeGameData(const int bytesRead)
{
    this->buffer.append(readBuf, bytesRead);

    const size_t delimeterPos = buffer.find("END");
    if (delimeterPos == std::string::npos)
        return;

    try
    {
        this->parseGameData(this->buffer.substr(0, delimeterPos).c_str());
    }
    catch (const char *e)
    {
        Client::printError("Game data processing error: " + std::string(e));
    }
    catch (...)
    {
        Client::printError("Game data processing error: unknown");
    }

    this->buffer.erase(0, delimeterPos + 3);
}

void Client::parseGameData(const char *data)
{
    int index = 0;

    const std::string snakeXStr = Client::deserealizeValue(data, &index);
    const std::string snakeYStr = Client::deserealizeValue(data + index, &index);
    const std::string heightStr = Client::deserealizeValue(data + index, &index);
    const std::string widthStr = Client::deserealizeValue(data + index, &index);
    const std::string fieldStr = Client::deserealizeValue(data + index, &index);
    if (snakeXStr.empty() || snakeYStr.empty() || heightStr.empty() || widthStr.empty() || fieldStr.empty())
        throw("Missing required fields");

    const int snakeX = std::stoi(snakeXStr);
    const int snakeY = std::stoi(snakeYStr);
    const int height = std::stoi(heightStr);
    const int width = std::stoi(widthStr);

    if (height * width != fieldStr.size())
        throw("Field size mismatch");

    if (height <= 0 || width <= 0 || height > 900 || width > 900)
        throw("Invalid dimensions");

    this->updateGameState(snakeX, snakeY, height, width, fieldStr);
}

void Client::updateGameState(int snakeX, int snakeY, int height, int width, const std::string &fieldStr)
{
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

    this->snakeX.store(snakeX);
    this->snakeY.store(snakeY);
    this->height.store(height);
    this->width.store(width);
}

// TLV format
std::string Client::deserealizeValue(const char *readBuf, int *index)
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

/// GETTERS

const std::vector<std::string> &Client::getGameField() const
{
    return this->gameField;
}

std::mutex &Client::getGameFieldMutex()
{
    return this->gameFieldMutex;
}

int Client::getWidth() const
{
    return this->width.load();
}

int Client::getHeight() const
{
    return this->height.load();
}

int Client::getSnakeX() const
{
    return this->snakeX.load();
}

int Client::getSnakeY() const
{
    return this->snakeY.load();
}

/// UTILS

void Client::printError(const std::string &str)
{
    std::cerr << str << std::endl;
}
