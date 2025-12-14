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
  void setStopFlag(bool value);

  const GameData* getGameData() const;
  const MapData* getMapData() const;
  std::mutex& getGameDataMutex();
  std::mutex& getMapDataMutex();
  int getStopFlag() const;

private:
  int tcpSocket;
  int udpSocket;
  sockaddr_in serverAddr;
  struct pollfd serverFd;
  pid_t localServerPid;
  int serverClientPipe[2];
  int clientServerPipe[2];

  // Accessed by drawer thread
  std::mutex gameDataMutex;
  const GameData* gameData;
  std::vector<uint8_t> gameDataBuffer;
  std::mutex mapDataMutex;
  const MapData* mapData;
  std::vector<uint8_t> mapDataBuffer;
  std::atomic<bool> stopFlag;

  void initConnections(const std::string& serverIP);
  void receiveGameData();
  void saveData(const uint8_t* data, size_t size);
  void startLocalServer();
  void stopLocalServer();
  void waitForServer(const std::string& serverIP);
};

#endif
