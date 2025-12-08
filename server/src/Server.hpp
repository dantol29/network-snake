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

  static std::string serializeValue(const std::string& value);

private:
  Game* game;
  int tcpServerFd;
  int udpServerFd;
  std::vector<struct pollfd> connectedClients;
  std::vector<struct pollfd> newConnections;
  std::vector<int> closedConnections;
  std::unordered_map<in_addr_t, int> addressToFd;
  char readBuf[10];

  std::string serializedGameField;
  std::string serializedHeight;
  std::string serializedWidth;

  void setupSocket(int socket);
  void initConnections();
  void acceptNewConnection();
  void addNewConnections();
  void closeConnection(const int fd);
  void removeClosedConnections();
  void sendGameData(const int fd) const;
  void receiveDataFromClient(const int fd);
  void handleSocketError(const int fd);
  std::string serializeGameField();
};

#endif
