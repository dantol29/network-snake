#include "Server.hpp"

#define SERV_PORT 3000
#define MAX_CLIENT_CONNECTIONS 10
#define NON_BLOCKING 0
#define BLOCKING -1

Server::Server(Game *game) : game(game)
{
    struct pollfd serverSocket;
    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(SERV_PORT);

    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1)
        onerror("Failed to create a socket");

    if (bind(serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        onerror("Failed to assign address to a socket");

    // make socket passive to accept incoming connection requests
    if (listen(serverFd, MAX_CLIENT_CONNECTIONS) == -1)
        onerror("Failed to make socket passive");

    if (fcntl(serverFd, F_SETFL, O_NONBLOCK) == -1)
        onerror("Failed to make socket non-blocking");

    serverSocket.fd = serverFd;
    serverSocket.events = POLLIN;
    serverSocket.revents = 0;
    connectedClients.push_back(serverSocket);

    lastSendTime = Clock::now();
}

void Server::start()
{
    while (poll(connectedClients.data(), connectedClients.size(), BLOCKING))
    {
        auto now = Clock::now();
        bool shouldSend = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSendTime).count() >= 50;
        if (shouldSend)
            serializedGameData = serializeGameData();

        int oldSize = connectedClients.size();
        for (int i = 0; i < oldSize; i++)
        {
            if (connectedClients[i].revents & POLLIN)
                receiveDataFromClient(connectedClients[i].fd, i);
            else if (connectedClients[i].revents & POLLOUT && shouldSend)
                sendGameData(connectedClients[i].fd);
            else if (connectedClients[i].revents & (POLLERR | POLLHUP | POLLNVAL))
                handleSocketError(connectedClients[i].fd, i);
        }

        removeClosedConnections();
        removeDeadSnakes();

        if (shouldSend)
            lastSendTime = now;
    }

    onerror("Poll error");
}

void Server::acceptNewConnection()
{
    struct pollfd fd;
    struct sockaddr_in cliAddr;

    socklen_t cliLen = sizeof(cliAddr);
    int clientFd = accept(serverFd, (struct sockaddr *)&cliAddr, &cliLen);
    if (clientFd >= 0)
    {
        if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
            onerror("Failed to make client fd non-blocking");

        fd.fd = clientFd;
        fd.events = POLLIN | POLLOUT;
        fd.revents = 0;
        connectedClients.push_back(fd);

        this->game->addSnake(clientFd);
    }
}

void Server::closeConnection(int fd)
{
    close(fd);
    this->game->removeSnake(fd);
    closedConnections.push_back(fd);
    printf("Client %d closed\n", fd);
}

void Server::removeClosedConnections()
{
    if (closedConnections.size() > 0)
    {
        for (auto it = closedConnections.begin(); it != closedConnections.end(); it++)
        {
            for (auto it2 = connectedClients.begin(); it2 != connectedClients.end(); it2++)
            {
                if (it2->fd == *it)
                {
                    connectedClients.erase(it2);
                    break;
                }
            }
        }

        closedConnections.clear();
    }
}

void Server::removeDeadSnakes()
{
    std::vector<int> deadSnakes = this->game->getDeadSnakes();
    for (int i = 0; i < deadSnakes.size(); i++)
        this->closeConnection(deadSnakes[i]);

    this->game->clearDeadSnakes();
}

// TODO: handle case when data is not sent in 1 write
void Server::sendGameData(int fd) const
{
    ssize_t bytesWritten = write(fd, serializedGameData.c_str(), serializedGameData.size());
    if (bytesWritten == -1 && (errno == EAGAIN || EWOULDBLOCK))
        std::cout << "Socket buffer is full..., trying again: " << fd << std::endl;
    else if (bytesWritten == -1)
        perror("write");
}

void Server::receiveDataFromClient(int fd, int index)
{
    if (index == 0)
        return acceptNewConnection();

    int bytesRead = read(fd, &readBuf, 10);
    if (bytesRead == 2)
    {
        game->setSnakeDirection(fd, (int)readBuf[0]);
        memset(readBuf, 0, 10);
    }
    else if (bytesRead == 0)
        closeConnection(fd);
    else
        printf("Error while reading!\n");
}

void Server::handleSocketError(int fd, int index)
{
    if (index == 0)
        onerror("Server socket crashed");

    std::cout << "Socket error: " << index << std::endl;
    closeConnection(fd);
}

// TLV format
std::string Server::serializeGameData()
{
    const std::string gameField = game->fieldToString();
    const std::string fieldSize = std::to_string(gameField.size());
    return std::to_string(fieldSize.size()) + fieldSize + gameField;
}
// TODO: handle case when data is not sent in 1 write
// void Server::sendGameData(int fd)
// {
//     // int alreadySent = fdToBytesWritten[fd];
//     // int dataSize = game->getFieldSize();
//     // if (alreadySent < dataSize)
//     // {
//     std::cout << "Sendind data" << std::endl;
//     ssize_t bytesWritten = write(fd, serializedGameData.c_str(), serializedGameData.size());
//     // if (bytesWritten > 0)
//     //     fdToBytesWritten[fd] += bytesWritten;
//     if (bytesWritten == -1 && (errno == EAGAIN || EWOULDBLOCK))
//         std::cout << "Socket buffer is full..., trying again: " << fd << std::endl;
//     else if (bytesWritten == -1)
//         perror("write");
//     // }
// }
