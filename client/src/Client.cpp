#include "Client.hpp"
#include <chrono>
#include <cstring>
#include <errno.h>
#include <string>
#include <thread>
#include <unistd.h>

#define BLOCKING -1
#define POLL_TIMEOUT_MS 10
#define SERVER_PORT 8080

Client::Client()
    : localServerPid(0), serverClientPipe{-1, -1}, clientServerPipe{-1, -1}, gameData(nullptr),
      stopFlag(false) {}

Client::~Client() {
  if (this->localServerPid > 0) 
    this->stopLocalServer();
  
  if (this->serverClientPipe[0] != -1)
    close(this->serverClientPipe[0]);
  if (this->serverClientPipe[1] != -1)
    close(this->serverClientPipe[1]);
  if (this->clientServerPipe[0] != -1)
    close(this->clientServerPipe[0]);
  if (this->clientServerPipe[1] != -1)
    close(this->clientServerPipe[1]);

  closeSockets();
}

void Client::closeSockets() {
  close(this->tcpSocket);
  close(this->udpSocket);
}

void Client::initConnections(const std::string& serverIP) {
  this->tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (this->tcpSocket < 0)
    throw "Socket init error";

  this->udpSocket = socket(AF_INET, SOCK_DGRAM, 0);

  this->serverAddr.sin_family = AF_INET;
  this->serverAddr.sin_port = htons(SERVER_PORT);
  inet_pton(AF_INET, serverIP.c_str(), &this->serverAddr.sin_addr);

  std::cout << "Connecting: " << serverIP << std::endl;

  if (connect(this->tcpSocket, (struct sockaddr*)&this->serverAddr, sizeof(this->serverAddr)) < 0)
    throw "Connect to server error";

  std::cout << "Connected" << std::endl;

  this->serverFd.fd = this->tcpSocket;
  this->serverFd.events = POLLIN;
  this->serverFd.revents = 0;
}

void Client::start(const std::string& serverIP, bool isSinglePlayer) {
  try {
    this->stopFlag.store(false);

    if (isSinglePlayer && !this->localServerPid) {
      this->startLocalServer();
      this->waitForServer(serverIP);
    }

    this->initConnections(serverIP);

    while (poll(&this->serverFd, 1, BLOCKING)) {
      if (this->serverFd.revents & POLLIN)
        receiveGameData();

      if (stopFlag.load())
        throw "Stop flag is set";
    }
  } catch (const char* msg) {
    // TODO: Show errors in game UI instead of just logging to stderr - display
    // user-friendly messages on screen and return to menu
    std::cerr << msg << std::endl;
  } catch (const std::string& msg) {
    // TODO: Show errors in game UI instead of just logging to stderr - display
    // user-friendly messages on screen and return to menu
    std::cerr << msg << std::endl;
  }

  this->stopFlag.store(true);

  std::cout << "Client has stopped" << std::endl;
  {
    std::lock_guard<std::mutex> lock(gameDataMutex);
    std::lock_guard<std::mutex> lock2(mapDataMutex);
    gameData = nullptr;
    mapData = nullptr;
    mapDataBuffer.clear();
    gameDataBuffer.clear();
  }

  closeSockets();
}

void readExact(int fd, uint8_t* buffer, size_t n) {
  size_t totalRead = 0;
  while (totalRead < n) {
    ssize_t bytesRead = read(fd, buffer + totalRead, n - totalRead);
    if (bytesRead < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        continue; // try again
      perror("read");
      throw "Error reading from server";
    }
    if (bytesRead == 0)
      throw "Socket closed by server";

    totalRead += bytesRead;
  }
}

// flatbuffer
void Client::receiveGameData() {
  // 1: read 4-byte length prefix
  uint32_t netSize;
  readExact(tcpSocket, reinterpret_cast<uint8_t*>(&netSize), sizeof(netSize));

  // 2. convert length to host byte order
  uint32_t size = ntohl(netSize);

  // 3: read data
  std::vector<uint8_t> buffer(size);
  readExact(tcpSocket, buffer.data(), size);

  saveData(buffer.data(), buffer.size());
}

void Client::saveData(const uint8_t* data, size_t size) {
  const Packet* packet = GetPacket(data);
  switch (packet->type()) {
  case MsgType_Game: {
    std::lock_guard<std::mutex> lock(gameDataMutex);

    gameDataBuffer.assign(data, data + size);
    const Packet* pckt = GetPacket(gameDataBuffer.data());
    gameData = pckt->data_as_GameData();
    break;
  }
  case MsgType_Map: {
    std::lock_guard<std::mutex> lock(mapDataMutex);

    mapDataBuffer.assign(data, data + size);
    const Packet* pckt = GetPacket(mapDataBuffer.data());
    mapData = pckt->data_as_MapData();
    break;
  }
  default:
    std::cerr << "Unknown packet type" << '\n';
  }
}

void Client::sendDirection(const enum actions newDirection) const {
  char writeBuf[2];
  writeBuf[0] = newDirection;
  writeBuf[1] = '\0';

  int bytesSent =
      sendto(this->udpSocket, &writeBuf, 2, 0, (struct sockaddr*)&this->serverAddr, sizeof(this->serverAddr));
  if (bytesSent != 2) // TODO: retry sending
    std::cout << "Error sending!" << std::endl;
}

/// GETTERS

const GameData* Client::getGameData() const { return this->gameData; }

const MapData* Client::getMapData() const { return this->mapData; }

std::mutex& Client::getGameDataMutex() { return this->gameDataMutex; }

std::mutex& Client::getMapDataMutex() { return this->mapDataMutex; }

int Client::getStopFlag() const { return this->stopFlag.load(); }

void Client::startLocalServer() {
  if (pipe(this->serverClientPipe) == -1)
    throw "Failed to create server-client pipe";

  if (pipe(this->clientServerPipe) == -1)
    throw "Failed to create client-server pipe";

  pid_t pid = fork();
  if (pid < 0) {
    throw "Failed to fork local server process";
  } else if (pid == 0) {
    close(this->serverClientPipe[0]);
    close(this->clientServerPipe[1]);

    if (dup2(this->serverClientPipe[1], STDERR_FILENO) == -1) {
      std::cerr << "Failed to redirect STDERR" << std::endl;
      exit(EXIT_FAILURE);
    }
    close(this->serverClientPipe[1]);

    if (dup2(this->clientServerPipe[0], STDIN_FILENO) == -1) {
      std::cerr << "Failed to redirect STDIN" << std::endl;
      exit(EXIT_FAILURE);
    }
    close(this->clientServerPipe[0]);

    chdir("../server");
    if (execl("./nibbler_server", "nibbler_server", std::to_string(DEFAULT_GAME_HEIGHT).c_str(),
              std::to_string(DEFAULT_GAME_WIDTH).c_str(), "maps/map.map", (char*)nullptr) == -1) {
      std::cerr << "Failed to execute local server: " << strerror(errno) << std::endl;
      exit(EXIT_FAILURE);
    }
  } else {
    close(this->serverClientPipe[1]);
    close(this->clientServerPipe[0]);

    int status;
    pid_t waited = waitpid(pid, &status, WNOHANG);
    if (waited == pid && WIFEXITED(status) && WEXITSTATUS(status) != 0) {
      // TODO: Read error from serverClientPipe[0]
      throw "Local server failed to start";
    }

    this->localServerPid = pid;
    std::cout << "Local server started with PID: " << pid << std::endl;
  }
}

void Client::stopLocalServer() {
  if (this->localServerPid > 0) {
    std::cout << "Stopping local server (PID: " << this->localServerPid << ")" << std::endl;

    if (this->clientServerPipe[1] != -1) {
      const char* shutdownMsg = "shutdown\n";
      write(this->clientServerPipe[1], shutdownMsg, strlen(shutdownMsg));
      close(this->clientServerPipe[1]);
      this->clientServerPipe[1] = -1;
    }

    waitpid(this->localServerPid, nullptr, 0);

    if (this->serverClientPipe[0] != -1) {
      close(this->serverClientPipe[0]);
      this->serverClientPipe[0] = -1;
    }

    this->localServerPid = 0;
  }
}

void Client::setStopFlag(bool value) { this->stopFlag.store(value); }

void Client::waitForServer(const std::string& serverIP) {
  const int maxRetries = 20;
  const int retryDelayMs = 100;

  sockaddr_in testAddr;
  testAddr.sin_family = AF_INET;
  testAddr.sin_port = htons(SERVER_PORT);
  inet_pton(AF_INET, serverIP.c_str(), &testAddr.sin_addr);

  for (int i = 0; i < maxRetries; ++i) {
    int testSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (testSocket < 0) {
      std::string error = "Failed to create test socket: " + std::string(strerror(errno));
      throw error;
    }

    int result = connect(testSocket, (struct sockaddr*)&testAddr, sizeof(testAddr));
    close(testSocket);

    if (result == 0) {
      std::cout << "Server is ready" << std::endl;
      return;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(retryDelayMs));
  }

  throw "Server failed to start within timeout";
}