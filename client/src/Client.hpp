#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../includes/nibbler.hpp"

class Client {
public:
  Client();
  Client(const Client& obj) = delete;
  Client& operator=(const Client& obj) = delete;
  ~Client();

  void start(const std::string& serverIP, bool isSinglePlayer = false);
  void sendDirection(const enum actions newDirection) const;
  void setIsDead(bool value);
  void setStopFlag(bool value);

  const std::vector<std::string>& getGameField() const;
  std::mutex& getGameFieldMutex();
  int getStopFlag() const;
  int getIsDead() const;
  int getWidth() const;
  int getHeight() const;
  int getSnakeX() const;
  int getSnakeY() const;

  static std::string deserealizeValue(const char* readBuf, int* index);

private:
  int tcpSocket;
  int udpSocket;
  sockaddr_in serverAddr;
  char readBuf[16384]; // 16 kb
  std::string buffer;
  struct pollfd serverFd;

  // Accessed by drawer thread
  std::mutex gameFieldMutex;
  std::vector<std::string> gameField;
  std::atomic<bool> stopFlag;
  std::atomic<bool> isDead;
  std::atomic<int> height;
  std::atomic<int> width;
  std::atomic<int> snakeX;
  std::atomic<int> snakeY;
  pid_t localServerPid;
  int serverClientPipe[2];
  int clientServerPipe[2];

  void initConnections(const std::string& serverIP);
  void startLocalServer();
  void stopLocalServer();
  void waitForServer(const std::string& serverIP);
  void receiveGameData();
  void deserealizeGameData(const int bytesRead);
  void parseGameData(const char* message);
  void updateGameState(int snakeX, int snakeY, int height, int width, const std::string& fieldStr);
};

#endif
