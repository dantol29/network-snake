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
}

void Server::start()
{
    while (1)
    {
        int events = poll(connectedClients.data(), connectedClients.size(), BLOCKING);
        if (events == -1)
            onerror("Poll error");

        if (events)
        {
            int size = connectedClients.size();
            for (int i = 0; i < size; i++)
            {
                if (connectedClients[i].revents & POLLIN)
                {
                    if (i == 0)
                        acceptNewConnection();
                    else
                        receiveDataFromClient(connectedClients[i].fd, i);
                }
                else if (connectedClients[i].revents & POLLOUT)
                    sendGameData(connectedClients[i].fd);
                else if (connectedClients[i].revents & (POLLERR | POLLHUP | POLLNVAL))
                {
                    if (i == 0)
                        onerror("Server socket crashed");
                    else
                        handleSocketError(connectedClients[i].fd, i);
                }
            }
            removeClosedConnections();
        }
    }
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
    }
}

void Server::closeConnection(int index, int fd)
{
    close(fd);
    closedConnections.push_back(index);
    printf("Client %d closed\n", index);
}

void Server::removeClosedConnections()
{
    if (closedConnections.size() > 0)
    {
        for (auto it = closedConnections.begin(); it != closedConnections.end(); it++)
            connectedClients.erase(connectedClients.begin() + *it);

        closedConnections.clear();
    }
}

void Server::sendGameData(int fd)
{
    int alreadySent = fdToBytesWritten[fd];
    int dataSize = game->getFieldSize();
    if (alreadySent < dataSize)
    {
        const std::string data = game->fieldToString().substr(alreadySent);
        std::cout << "Sendind data: " << data << std::endl;
        std::cout << "Size: " << dataSize - alreadySent << std::endl;

        ssize_t bytesWritten = write(fd, data.c_str(), dataSize - alreadySent);
        if (bytesWritten > 0)
            fdToBytesWritten[fd] += bytesWritten;
        else if (bytesWritten == -1 && (errno == EAGAIN || EWOULDBLOCK))
            std::cout << "Socket buffer is full..., trying again: " << fd << std::endl;
        else
            std::cout << "Failed to write to: " << fd << std::endl;
    }
}

void Server::receiveDataFromClient(int fd, int index)
{
    char buf[1024];
    int bytesRead = read(fd, &buf, 1024);
    if (bytesRead == 0)
        closeConnection(index, fd);
    else if (bytesRead > 0)
    {
        printf("Received: %s\n", buf);
    }
    else
    {
        printf("Error while reading!\n");
    }
}

void Server::handleSocketError(int fd, int index)
{
    std::cout << "Socket error: " << index << std::endl;
    closeConnection(index, fd);
}
