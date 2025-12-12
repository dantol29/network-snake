#include "Server.hpp"
#include <sys/uio.h>

#define SERV_PORT 8080
#define MAX_CLIENT_CONNECTIONS 10
#define NON_BLOCKING 0
#define BLOCKING -1

Server::Server(Game* game) : game(game) {
  this->serializedHeight = Server::serializeValue(std::to_string(game->getHeight()));
  this->serializedWidth = Server::serializeValue(std::to_string(game->getWidth()));
}

void Server::setupSocket(int socket) {
  int flag = 1; // Disable Nagle's Algorithm
  setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
}

Server::~Server() {
  std::cout << "Server Destructor called!" << std::endl;
  for (const auto client : connectedClients) {
    close(client.fd);
  }
}

void Server::initConnections() {
  struct sockaddr_in serverAddr;

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr.sin_port = htons(SERV_PORT);

  this->tcpServerFd = socket(AF_INET, SOCK_STREAM, 0);
  if (this->tcpServerFd == -1)
    throw "Failed to create a tcp socket";

  this->setupSocket(this->tcpServerFd);

  if (bind(this->tcpServerFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    throw "Failed to assign address to a tcp socket";

  // make socket passive to accept incoming connection requests
  if (listen(this->tcpServerFd, MAX_CLIENT_CONNECTIONS) == -1)
    throw "Failed to make tcp socket passive";

  if (fcntl(this->tcpServerFd, F_SETFL, O_NONBLOCK) == -1)
    throw "Failed to make tcp socket non-blocking";

  this->udpServerFd = socket(AF_INET, SOCK_DGRAM, 0);
  if (this->udpServerFd == -1)
    throw "Failed to create a udp socket";

  if (bind(this->udpServerFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    throw "Failed to assign address to a udp socket";

  pollfd tcpPoll = {tcpServerFd, POLLIN, 0};
  pollfd udpPoll = {udpServerFd, POLLIN, 0};
  pollfd stdin = {STDIN_FILENO, POLLIN, 0};
  connectedClients.push_back(tcpPoll);
  connectedClients.push_back(udpPoll);
  connectedClients.push_back(stdin);
}

void Server::start() {
  try {
    this->initConnections();

    while (!this->game->getStopFlag()) {
      if (poll(connectedClients.data(), connectedClients.size(), BLOCKING) < 0)
        break;

      bool shouldSend = this->game->getIsDataUpdated();
      if (shouldSend) {
        this->game->setIsDataUpdated(false);
        constructResponse();
      }

      for (const auto client : connectedClients) {
        if (client.revents & POLLIN)
          receiveDataFromClient(client.fd);
        else if (client.revents & POLLOUT && shouldSend)
          sendGameData(client.fd);
        else if (client.revents & (POLLERR | POLLHUP | POLLNVAL))
          handleSocketError(client.fd);
      }

      if (this->closedConnections.size())
        removeClosedConnections();
      if (this->newConnections.size())
        addNewConnections();
    }
  } catch (const char* msg) {
    std::cerr << msg << std::endl;
  }

  this->game->stop();
}

void Server::acceptNewConnection() {
  struct sockaddr_in cliAddr;
  socklen_t cliLen = sizeof(cliAddr);

  int clientFd = accept(this->tcpServerFd, (struct sockaddr*)&cliAddr, &cliLen);
  if (clientFd >= 0) {
    if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1) {
      std::cerr << "Failed to make client fd non-blocking" << std::endl;
      return;
    }

    struct pollfd fd;
    fd.fd = clientFd;
    fd.events = POLLIN | POLLOUT;
    fd.revents = 0;
    this->newConnections.push_back(fd);

    this->addressToFd[cliAddr.sin_addr.s_addr] = clientFd;

    this->game->addSnake(clientFd);
  }
}

void Server::closeConnection(const int fd) {
  close(fd);
  this->closedConnections.push_back(fd);
  this->game->removeSnake(fd);
  std::cout << "Client removed: " << fd << std::endl;
}

void Server::addNewConnections() {
  for (const auto connection : newConnections)
    this->connectedClients.push_back(connection);

  this->newConnections.clear();
}

void Server::removeClosedConnections() {
  for (const int fd : closedConnections) {
    for (auto it = connectedClients.begin(); it != connectedClients.end(); it++) {
      if (it->fd == fd) {
        connectedClients.erase(it);
        break;
      }
    }
  }

  closedConnections.clear();
}

void Server::constructResponse() {
  flatbuffers::FlatBufferBuilder builder(1024);

  auto gameData = game->buildGameData(builder);
  builder.Finish(gameData);

  // size of data in bytes
  uint32_t netSize = htonl(builder.GetSize());

  // sent size of data first and then data
  iov[0].iov_base = &netSize;
  iov[0].iov_len = sizeof(netSize);
  iov[1].iov_base = builder.GetBufferPointer();
  iov[1].iov_len = builder.GetSize();
}

// TCP
void Server::sendGameData(const int fd) const {
  ssize_t bytesWritten = writev(fd, iov, 2);
  if (bytesWritten == -1 && (errno == EAGAIN || EWOULDBLOCK))
    std::cout << "Socket buffer is full, could not sent game data: " << fd << std::endl;
  else if (bytesWritten == -1)
    perror("write");
}

// UDP
void Server::receiveDataFromClient(const int fd) {
  if (fd == STDIN_FILENO)
    throw "Server stopped by admin";

  if (fd == this->tcpServerFd)
    return acceptNewConnection();

  if (fd == this->udpServerFd) {
    std::cout << "receiving data from client" << '\n';
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int n = recvfrom(this->udpServerFd, readBuf, 2, 0, (sockaddr*)&clientAddr, &clientAddrLen);
    if (n == 2)
      game->updateSnakeDirection(this->addressToFd[clientAddr.sin_addr.s_addr], (int)readBuf[0]);
    else
      std::cout << "Failed to receive data from client" << std::endl;
    return;
  }

  closeConnection(fd);
}

void Server::handleSocketError(const int fd) {
  if (fd == this->tcpServerFd)
    throw "Server socket crashed";

  std::cout << "Socket error: " << fd << std::endl;
  closeConnection(fd);
}