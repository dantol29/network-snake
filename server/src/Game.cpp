#include "Game.hpp"
#include "Snake.hpp"
#include <chrono>
#include <fstream>

#define MAX_FOOD_SPAWN_TRIES 50

bool has_invalid_chars(const std::string& line);

using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;

TimePoint lastMoveTime = Clock::now();
TimePoint lastEatTime = Clock::now();

Game::Game(const int height, const int width, const std::string& mapPath)
    : stopFlag(false), snakeCount(0), foodCount(0) {

  try {
    if (mapPath.empty())
      throw "Map is not defined";

    loadGameMap(mapPath);
  } catch (const char* err) {
    std::cerr << err << std::endl;
    std::cout << "Fallback to an empty map" << std::endl;

    this->writableField.clear();
    for (int i = 0; i < height; i++)
      this->writableField.push_back(std::string(width, FLOOR_SYMBOL));
  }

  updateReadableField();
  this->height.store(writableField.size());
  this->width.store(writableField[0].size());

  std::cout << "height: " << writableField.size() << ", width: " << writableField[0].size() << '\n';
  printField();

  srand(time(NULL)); // init random generator
}

Game::~Game() {
  std::cout << "Game destr called" << std::endl;

  for (auto it = this->snakes.begin(); it != this->snakes.end(); it++)
    delete it->second;
}

void Game::stop() { this->stopFlag.store(true); }

void Game::loadGameMap(const std::string& mapFile) {
  std::ifstream file(mapFile);
  if (!file.is_open())
    throw "Error opening map";

  int width = 0;
  std::string line;
  while (getline(file, line)) {
    if (!width)
      width = line.size();

    if (has_invalid_chars(line))
      throw "Invalid characters";

    if (line.size() != width)
      throw "Invalid line width";

    writableField.push_back(line);
  }

  if (!file.eof())
    throw "Error reading assets file";

  file.close();
}

void Game::start() {
  auto nextMoveTime = Clock::now() + std::chrono::milliseconds(SNAKE_SPEED);

  while (!stopFlag.load()) {
    auto now = Clock::now();
    if (now >= nextMoveTime) {
      this->moveSnakes();
      this->spawnFood();

      this->updateReadableField();
      this->setIsDataUpdated(true);
      nextMoveTime = now + std::chrono::milliseconds(SNAKE_SPEED);
    }

    std::this_thread::sleep_until(nextMoveTime);
  }
}

void Game::spawnFood() {
  if (!(this->snakeCount.load() * 2 > this->foodCount))
    return;

  for (int i = 0; i < MAX_FOOD_SPAWN_TRIES; i++) {
    int r1 = rand();
    int r2 = rand();
    int x = r1 % (this->width - 1);
    int y = r2 % (this->height - 1);

    if (this->writableField[y][x] == FLOOR_SYMBOL) {
      this->writableField[y][x] = 'F';
      ++this->foodCount;
      return;
    }
  }
}

void Game::moveSnakes() {
  std::lock_guard<std::mutex> lock(this->snakesMutex);
  for (auto it = this->snakes.begin(); it != this->snakes.end();) {
    it->second->moveSnake(&this->writableField);
    if (it->second->getIsDead()) {
      it->second->cleanup(&writableField);
      delete it->second;
      it = snakes.erase(it);
      --this->snakeCount;
    } else
      ++it;
  }
}

void Game::addSnake(const int clientFd) {
  std::lock_guard<std::mutex> lock1(snakesMutex);
  Snake* newSnake = new Snake(this, clientFd);
  this->snakes[clientFd] = newSnake;

  ++this->snakeCount;
}

void Game::removeSnake(int fd) {
  std::lock_guard<std::mutex> lock(this->snakesMutex);

  auto snake = this->snakes.find(fd);
  if (snake != this->snakes.end() && snake->second) {
    snake->second->cleanup(&this->writableField);
    delete snake->second;
    this->snakes.erase(snake);
    --this->snakeCount;
  }
}

void Game::updateReadableField() {
  std::lock_guard<std::mutex> lock(this->readableFieldMutex);

  this->readableField = this->writableField;
}

void Game::updateSnakeDirection(const int fd, const int dir) {
  std::lock_guard<std::mutex> lock(this->snakesMutex);

  auto it = this->snakes.find(fd);
  if (it != this->snakes.end() && it->second)
    this->snakes[fd]->setDirection(dir);
}

void Game::decreaseFood() {
  if (this->foodCount > 0)
    this->foodCount--;
}

void Game::setIsDataUpdated(bool value) { this->isDataUpdated.store(value); }

/// GETTERS

t_coordinates Game::getSnakeHead(const int fd) {
  std::lock_guard<std::mutex> lock(snakesMutex);

  t_coordinates head = {0};
  auto it = this->snakes.find(fd);
  if (it != this->snakes.end() && it->second)
    head = it->second->getHead();

  return head;
}

int Game::getHeight() const { return this->height.load(); }

int Game::getWidth() const { return this->width.load(); }

bool Game::getStopFlag() const { return this->stopFlag.load(); }

bool Game::getIsDataUpdated() const { return this->isDataUpdated.load(); }

/// UTILS

std::string Game::fieldToString() {
  std::lock_guard<std::mutex> lock(this->readableFieldMutex);

  std::string data;

  for (int i = 0; i < readableField.size(); i++)
    data += readableField[i];

  return data;
}

void Game::printField() {
  std::lock_guard<std::mutex> lock(this->readableFieldMutex);

  printf("\n\n");
  for (int i = 0; i < readableField.size(); i++)
    printf("%3d:%s\n", i, readableField[i].c_str());
  printf("\n\n");
}

bool has_invalid_chars(const std::string& line) {
  for (char c : line) {
    if (c != FLOOR_SYMBOL && c != 'W' && c != 'V' && c != 'F' && c != 'H' && c != 'B' && c != 'T')
      return true;
  }
  return false;
}