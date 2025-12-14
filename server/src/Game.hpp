#ifndef GAME_HPP
#define GAME_HPP

#include "../includes/nibbler.hpp"

using xCoord = int;
using yCoord = int;

class Snake;

class Game {
public:
  Game(int height, int width, const std::string& mapPath);
  Game(const Game& obj) = delete;
  Game& operator=(const Game& obj) = delete;
  ~Game();

  void start();
  void stop();

  void removeFood(int x, int y);
  void addSnake(int fd);
  void removeSnake(int fd);
  void updateSnakeDirection(int fd, int dir);
  void setIsDataUpdated(bool value);

  int getHeight() const;
  int getWidth() const;
  bool getStopFlag() const;
  bool getIsDataUpdated() const;
  flatbuffers::Offset<Packet> serializeGameData(flatbuffers::FlatBufferBuilder& builder);
  flatbuffers::Offset<Packet> serializeMapData(flatbuffers::FlatBufferBuilder& builder, int fd);

private:
  std::vector<std::string> writableField;

  // Used by another thread
  std::atomic<int> height;
  std::atomic<int> width;
  std::atomic<bool> stopFlag;
  std::atomic<bool> isDataUpdated;

  std::mutex snakesMutex;
  std::unordered_map<int, Snake*> snakes;

  std::mutex foodMutex;
  std::vector<std::pair<xCoord, yCoord>> food;

  std::mutex readableFieldMutex;
  std::vector<std::string> readableField;

  void spawnFood();
  void moveSnakes();
  void updateReadableField();
  void printField();
  State getSnakeState(const int fd);
  void loadGameMap(const std::string& mapFile);
};

#endif
