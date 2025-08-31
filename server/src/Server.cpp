#include "Server.hpp"

#define SERV_PORT 8080
#define MAX_CLIENT_CONNECTIONS 10
#define NON_BLOCKING 0
#define BLOCKING -1

Server::Server(Game *game) : game(game)
{
    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(SERV_PORT);

    this->tcpServerFd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->tcpServerFd == -1)
        onerror("Failed to create a tcp socket");

    this->setupSocket();

    if (bind(this->tcpServerFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        onerror("Failed to assign address to a tcp socket");

    // make socket passive to accept incoming connection requests
    if (listen(this->tcpServerFd, MAX_CLIENT_CONNECTIONS) == -1)
        onerror("Failed to make tcp socket passive");

    if (fcntl(this->tcpServerFd, F_SETFL, O_NONBLOCK) == -1)
        onerror("Failed to make tcp socket non-blocking");

    this->udpServerFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (this->udpServerFd == -1)
        onerror("Failed to create a udp socket");

    if (bind(this->udpServerFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        onerror("Failed to assign address to a udp socket");

    pollfd tcpPoll = {tcpServerFd, POLLIN, 0};
    pollfd udpPoll = {udpServerFd, POLLIN, 0};
    connectedClients.push_back(tcpPoll);
    connectedClients.push_back(udpPoll);

    this->serializedHeight = Server::serializeValue(std::to_string(game->getHeight()));
    this->serializedWidth = Server::serializeValue(std::to_string(game->getWidth()));
}

void Server::setupSocket()
{
    int flag = 1; // Disable Nagle's Algorithm
    setsockopt(this->tcpServerFd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
}

Server::~Server()
{
    std::cout << "Destructor called!" << std::endl;
    close(this->tcpServerFd);
    close(this->udpServerFd);
}

void Server::start()
{
    while (poll(connectedClients.data(), connectedClients.size(), BLOCKING))
    {
        if (this->game->getStopFlag())
            return;

        bool shouldSend = this->game->getIsDataUpdated();
        if (shouldSend)
        {
            this->game->setIsDataUpdated(false);
            this->serializedGameField = serializeGameField();
        }

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
    }

    onerror("Poll error");
}

void Server::acceptNewConnection()
{
    struct sockaddr_in cliAddr;
    socklen_t cliLen = sizeof(cliAddr);

    int clientFd = accept(tcpServerFd, (struct sockaddr *)&cliAddr, &cliLen);
    if (clientFd >= 0)
    {
        if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
            onerror("Failed to make client fd non-blocking");

        struct pollfd fd;
        fd.fd = clientFd;
        fd.events = POLLIN | POLLOUT;
        fd.revents = 0;
        connectedClients.push_back(fd);

        this->addressToFd[cliAddr.sin_addr.s_addr] = clientFd;

        this->game->addSnake(clientFd);

        // game size might change on new player add
        this->serializedHeight = Server::serializeValue(std::to_string(game->getHeight()));
        this->serializedWidth = Server::serializeValue(std::to_string(game->getWidth()));
    }
}

void Server::closeConnection(const int fd)
{
    close(fd);
    closedConnections.push_back(fd);
    this->game->addDeadSnake(fd);
    std::cout << "Client removed: " << fd << std::endl;
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

// TCP
void Server::sendGameData(const int fd) const
{
    struct coordinates head = this->game->getSnakeHead(fd);
    const std::string headX = serializeValue(std::to_string(head.x));
    const std::string headY = serializeValue(std::to_string(head.y));
    const std::string serializedGameData = headX + headY + this->serializedGameField + "END";

    ssize_t bytesWritten = write(fd, serializedGameData.c_str(), serializedGameData.size());
    if (bytesWritten == -1 && (errno == EAGAIN || EWOULDBLOCK))
        std::cout << "Socket buffer is full, could not sent game data: " << fd << std::endl;
    else if (bytesWritten == -1)
        perror("write");
}

// UDP
void Server::receiveDataFromClient(const int fd, const int index)
{
    if (index == 0)
        return acceptNewConnection();

    if (index == 1)
    {
        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int n = recvfrom(this->udpServerFd, readBuf, 2, 0, (sockaddr *)&clientAddr, &clientAddrLen);
        if (n == 2)
            game->updateSnakeDirection(this->addressToFd[clientAddr.sin_addr.s_addr], (int)readBuf[0]);
        else
            std::cout << "ERROR!" << std::endl;
        return;
    }

    closeConnection(fd);
}

void Server::handleSocketError(const int fd, const int index)
{
    if (index == 0)
        onerror("Server socket crashed");

    std::cout << "Socket error: " << index << std::endl;
    closeConnection(fd);
}

// TLV format
std::string Server::serializeGameField()
{
    const std::string field = Server::serializeValue(game->fieldToString());
    return this->serializedHeight + this->serializedWidth + field;
}

std::string Server::serializeValue(const std::string &value)
{
    const std::string len = std::to_string(value.size());
    const std::string lenSize = std::to_string(len.size());

    return lenSize + len + value;
}
