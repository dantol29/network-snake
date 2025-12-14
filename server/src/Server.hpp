#ifndef SERVER_HPP
#define SERVER_HPP

#include "Game.hpp"

class Game;

class Server {
public:
  Server(Game* game);
  Server(const Server& obj) = delete;
  Server& operator=(const Server& obj) = delete;
  ~Server();

  void start();

private:
  Game* game;
  int tcpServerFd;
  int udpServerFd;
  std::vector<struct pollfd> connectedClients;
  std::vector<struct pollfd> newConnections;
  std::vector<int> closedConnections;
  std::unordered_map<in_addr_t, int> addressToFd;
  struct iovec iovGame[2];
  struct iovec iovMap[2];
  std::vector<uint8_t> mapBuffer;
  uint32_t mapSizeNetwork;
  std::vector<uint8_t> gameBuffer;
  uint32_t gameSizeNetwork;

  void setupSocket(int socket);
  void initConnections();
  void acceptNewConnection();
  void addNewConnections();
  void closeConnection(const int fd);
  void removeClosedConnections();
  void sendGameData(const int fd) const;
  void sendMapData(const int fd);
  void receiveDataFromClient(const int fd);
  void handleSocketError(const int fd);
  void constructGameData();
  void constructMapData(int fd);
};

#endif
