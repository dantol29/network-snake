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
    : stopFlag(false), isDead(false), height(0), width(0), snakeX(0), snakeY(0),
      localServerPid(0), serverClientPipe{-1, -1}, clientServerPipe{-1, -1} {}

Client::~Client()
{
  close(this->tcpSocket);
  close(this->udpSocket);
  if (this->serverClientPipe[0] != -1)
    close(this->serverClientPipe[0]);
  if (this->serverClientPipe[1] != -1)
    close(this->serverClientPipe[1]);
  if (this->clientServerPipe[0] != -1)
    close(this->clientServerPipe[0]);
  if (this->clientServerPipe[1] != -1)
    close(this->clientServerPipe[1]);
}

void Client::initConnections(const std::string &serverIP)
{
  this->tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (this->tcpSocket < 0)
    throw "Socket init error";

  this->udpSocket = socket(AF_INET, SOCK_DGRAM, 0);

  this->serverAddr.sin_family = AF_INET;
  this->serverAddr.sin_port = htons(SERVER_PORT);
  inet_pton(AF_INET, serverIP.c_str(), &this->serverAddr.sin_addr);

  std::cout << "Connecting: " << serverIP << std::endl;

  if (connect(this->tcpSocket, (struct sockaddr *)&this->serverAddr,
              sizeof(this->serverAddr)) < 0)
    throw "Connect to server error";

  std::cout << "Connected" << std::endl;

  this->serverFd.fd = this->tcpSocket;
  this->serverFd.events = POLLIN;
  this->serverFd.revents = 0;
}

void Client::start(const std::string &serverIP, bool isSinglePlayer)
{
  try
  {
    this->isDead.store(false);
    this->stopFlag.store(false);

    if (isSinglePlayer)
    {
      this->startLocalServer();
      this->waitForServer(serverIP);
    }

    this->initConnections(serverIP);

    while (poll(&this->serverFd, 1, BLOCKING))
    {
      if (this->serverFd.revents & POLLIN)
        receiveGameData();

      if (stopFlag.load())
        throw "Stop flag is set";
    }
  }
  catch (const char *msg)
  {
    // TODO: Show errors in game UI instead of just logging to stderr - display
    // user-friendly messages on screen and return to menu
    std::cerr << msg << std::endl;
  }
  catch (const std::string &msg)
  {
    // TODO: Show errors in game UI instead of just logging to stderr - display
    // user-friendly messages on screen and return to menu
    std::cerr << msg << std::endl;
  }

  this->stopFlag.store(true);

  if (isSinglePlayer)
  {
    this->stopLocalServer();
  }

  std::cout << "Client has stopped" << std::endl;
}

void Client::receiveGameData()
{
  // static auto lastReadTime = std::chrono::steady_clock::now();

  int bytesRead = read(this->tcpSocket, &this->readBuf, 16384);
  if (bytesRead > 0)
  {
    // auto currentTime = std::chrono::steady_clock::now();
    // auto elapsed =
    // std::chrono::duration_cast<std::chrono::milliseconds>(currentTime -
    // lastReadTime).count(); std::cout << "(" << elapsed << " ms)" <<
    // std::endl; lastReadTime = currentTime;

    deserealizeGameData(bytesRead);
  }
  else if (bytesRead == 0)
    throw "Server closed connection";
  else
    throw "Error while reading from server socket";
}

void Client::sendDirection(const enum actions newDirection) const
{
  char writeBuf[2];
  writeBuf[0] = newDirection;
  writeBuf[1] = '\0';

  int bytesSent =
      sendto(this->udpSocket, &writeBuf, 2, 0,
             (struct sockaddr *)&this->serverAddr, sizeof(this->serverAddr));
  if (bytesSent != 2) // TODO: retry sending
    std::cout << "Error sending!" << std::endl;
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
    std::cerr << "Game data processing error: " + std::string(e) << std::endl;
  }
  catch (...)
  {
    std::cerr << "Game data processing error: unknown" << std::endl;
  }

  this->buffer.erase(0, delimeterPos + 3);
}

// Data format:
// SNAKE_X | SNAKE_Y | GAME_HEIGHT | GAME_WIDTH | GAME_FIELD
void Client::parseGameData(const char *data)
{
  int index = 0;

  const std::string snakeXStr = Client::deserealizeValue(data, &index);
  const std::string snakeYStr = Client::deserealizeValue(data + index, &index);
  const std::string heightStr = Client::deserealizeValue(data + index, &index);
  const std::string widthStr = Client::deserealizeValue(data + index, &index);
  const std::string fieldStr = Client::deserealizeValue(data + index, &index);
  if (snakeXStr.empty() || snakeYStr.empty() || heightStr.empty() ||
      widthStr.empty() || fieldStr.empty())
    throw "Missing required fields";

  const int snakeX = std::stoi(snakeXStr);
  const int snakeY = std::stoi(snakeYStr);
  const int height = std::stoi(heightStr);
  const int width = std::stoi(widthStr);

  if ((size_t)(height * width) != fieldStr.size())
    throw "Field size mismatch";

  if (height <= 0 || width <= 0 || height > 900 || width > 900)
    throw "Invalid dimensions";

  this->updateGameState(snakeX, snakeY, height, width, fieldStr);
}

void Client::updateGameState(int snakeX, int snakeY, int height, int width,
                             const std::string &fieldStr)
{
  std::lock_guard<std::mutex> lock(this->gameFieldMutex);

  this->gameField.clear();

  std::string row;
  for (int y = 0; y < height; y++)
  {
    row.clear();
    for (int x = 0; x < width; x++)
      row += fieldStr[x + y * width];

    this->gameField.push_back(row);
  }

  this->snakeX.store(snakeX);
  this->snakeY.store(snakeY);
  this->height.store(height);
  this->width.store(width);

  if (snakeX == 0 && snakeY == 0)
  {
    this->isDead.store(true);
    close(this->tcpSocket);
    close(this->udpSocket);
    throw "Snake is dead";
  }
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

std::mutex &Client::getGameFieldMutex() { return this->gameFieldMutex; }

int Client::getWidth() const { return this->width.load(); }

int Client::getHeight() const { return this->height.load(); }

int Client::getSnakeX() const { return this->snakeX.load(); }

int Client::getSnakeY() const { return this->snakeY.load(); }

int Client::getStopFlag() const { return this->stopFlag.load(); }

int Client::getIsDead() const { return this->isDead.load(); }

void Client::setIsDead(bool value) { this->isDead.store(value); }

void Client::startLocalServer()
{
  if (pipe(this->serverClientPipe) == -1)
  {
    throw "Failed to create server-client pipe";
  }
  if (pipe(this->clientServerPipe) == -1)
  {
    close(this->serverClientPipe[0]);
    close(this->serverClientPipe[1]);
    throw "Failed to create client-server pipe";
  }

  pid_t pid = fork();
  if (pid < 0)
  {
    close(this->serverClientPipe[0]);
    close(this->serverClientPipe[1]);
    close(this->clientServerPipe[0]);
    close(this->clientServerPipe[1]);
    throw "Failed to fork local server process";
  }
  else if (pid == 0)
  {
    close(this->serverClientPipe[0]);
    close(this->clientServerPipe[1]);

    if (dup2(this->serverClientPipe[1], STDERR_FILENO) == -1)
    {
      std::cerr << "Failed to redirect STDERR" << std::endl;
      exit(EXIT_FAILURE);
    }
    close(this->serverClientPipe[1]);

    if (dup2(this->clientServerPipe[0], STDIN_FILENO) == -1)
    {
      std::cerr << "Failed to redirect STDIN" << std::endl;
      exit(EXIT_FAILURE);
    }
    close(this->clientServerPipe[0]);

    chdir("../server");
    if (execl("./nibbler_server", "nibbler_server",
              std::to_string(DEFAULT_GAME_HEIGHT).c_str(),
              std::to_string(DEFAULT_GAME_WIDTH).c_str(),
              (char *)nullptr) == -1)
    {
      std::cerr << "Failed to execute local server: " << strerror(errno)
                << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  else
  {
    close(this->serverClientPipe[1]);
    close(this->clientServerPipe[0]);

    int status;
    pid_t waited = waitpid(pid, &status, WNOHANG);
    if (waited == pid && WIFEXITED(status) && WEXITSTATUS(status) != 0)
    {
      // TODO: Read error from serverClientPipe[0] and show user-friendly error
      // in GUI
      close(this->serverClientPipe[0]);
      close(this->clientServerPipe[1]);
      throw "Local server failed to start";
    }

    this->localServerPid = pid;
    std::cout << "Local server started with PID: " << pid << std::endl;
  }
}

void Client::stopLocalServer()
{
  if (this->localServerPid > 0)
  {
    std::cout << "Stopping local server (PID: " << this->localServerPid << ")"
              << std::endl;

    if (this->clientServerPipe[1] != -1)
    {
      const char *shutdownMsg = "shutdown\n";
      write(this->clientServerPipe[1], shutdownMsg, strlen(shutdownMsg));
      close(this->clientServerPipe[1]);
      this->clientServerPipe[1] = -1;
    }

    waitpid(this->localServerPid, nullptr, 0);

    if (this->serverClientPipe[0] != -1)
    {
      close(this->serverClientPipe[0]);
      this->serverClientPipe[0] = -1;
    }

    this->localServerPid = 0;
  }
}

void Client::setStopFlag(bool value) { this->stopFlag.store(value); }

void Client::waitForServer(const std::string &serverIP)
{
  const int maxRetries = 20;
  const int retryDelayMs = 100;

  sockaddr_in testAddr;
  testAddr.sin_family = AF_INET;
  testAddr.sin_port = htons(SERVER_PORT);
  inet_pton(AF_INET, serverIP.c_str(), &testAddr.sin_addr);

  for (int i = 0; i < maxRetries; ++i)
  {
    int testSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (testSocket < 0)
    {
      std::string error =
          "Failed to create test socket: " + std::string(strerror(errno));
      throw error;
    }

    int result =
        connect(testSocket, (struct sockaddr *)&testAddr, sizeof(testAddr));
    close(testSocket);

    if (result == 0)
    {
      std::cout << "Server is ready" << std::endl;
      return;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(retryDelayMs));
  }

  throw "Server failed to start within timeout";
}