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

Game::Game(int height, int width, const std::string& mapPath) : stopFlag(false) {

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

    if ((int)line.size() != width)
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
  std::lock_guard<std::mutex> lock(foodMutex);

  if (food.size() > 2)
    return;

  for (int i = 0; i < MAX_FOOD_SPAWN_TRIES; i++) {
    int r1 = rand();
    int r2 = rand();
    int x = r1 % (this->width - 1);
    int y = r2 % (this->height - 1);

    if (this->writableField[y][x] == FLOOR_SYMBOL) {
      this->writableField[y][x] = 'F';
      this->food.emplace_back(std::make_pair(x, y));
      return;
    }
  }
}

void Game::moveSnakes() {
  std::lock_guard<std::mutex> lock(this->snakesMutex);
  for (auto it = this->snakes.begin(); it != this->snakes.end();) {
    it->second->moveSnake(&this->writableField);
    if (it->second->getState() == State_Dead) {
      it->second->cleanup(&writableField);
      delete it->second;
      it = snakes.erase(it);
    } else
      ++it;
  }
}

void Game::addSnake(int clientFd) {
  std::lock_guard<std::mutex> lock1(snakesMutex);
  Snake* newSnake = new Snake(this, clientFd);
  this->snakes[clientFd] = newSnake;
}

void Game::removeSnake(int fd) {
  std::lock_guard<std::mutex> lock(this->snakesMutex);

  auto snake = this->snakes.find(fd);
  if (snake != this->snakes.end() && snake->second) {
    snake->second->cleanup(&this->writableField);
    delete snake->second;
    this->snakes.erase(snake);
  }
}

void Game::updateReadableField() {
  std::lock_guard<std::mutex> lock(this->readableFieldMutex);

  this->readableField = this->writableField;
}

void Game::updateSnakeDirection(int fd, int dir) {
  std::lock_guard<std::mutex> lock(this->snakesMutex);

  auto it = this->snakes.find(fd);
  if (it != this->snakes.end() && it->second)
    this->snakes[fd]->setDirection(dir);
}

void Game::removeFood(int x, int y) {
  std::lock_guard<std::mutex> lock(foodMutex);

  for (auto it = food.begin(); it != food.end(); ++it) {
    if (it->first == x && it->second == y) {
      food.erase(it);
      return;
    }
  }
}

flatbuffers::Offset<Packet> Game::serializeGameData(flatbuffers::FlatBufferBuilder& builder) {
  std::lock_guard<std::mutex> lock(snakesMutex);

  std::vector<flatbuffers::Offset<SnakeObj>> snakesVec;
  snakesVec.reserve(snakes.size());

  for (auto snake : snakes) {
    std::vector<Pos> bodyVec;
    for (auto pos : snake.second->getBody())
      bodyVec.emplace_back(Pos(pos.x, pos.y));

    auto body = builder.CreateVectorOfStructs(bodyVec);
    auto state = snake.second->getState();
    auto id = snake.first;
    auto snakeObject = CreateSnakeObj(builder, id, state, body);
    snakesVec.emplace_back(snakeObject);
  }

  std::vector<Pos> foodVec;
  foodVec.reserve(food.size());

  for (auto f : food) {
    foodVec.emplace_back(Pos(f.first, f.second));
  }

  auto snakesData = builder.CreateVector(snakesVec);
  auto foodData = builder.CreateVectorOfStructs(foodVec);
  auto gameData = CreateGameData(builder, snakesData, foodData);
  return CreatePacket(builder, MsgType_Game, MsgUnion_GameData, gameData.Union());
}

flatbuffers::Offset<Packet> Game::serializeMapData(flatbuffers::FlatBufferBuilder& builder, int fd) {
  std::lock_guard<std::mutex> lock(readableFieldMutex);

  std::vector<flatbuffers::Offset<Row>> rows;
  rows.reserve(readableField.size());

  for (auto row : readableField) {
    std::vector<int8_t> tiles;
    tiles.reserve(row.size());

    for (char tile : row) {
      if (tile == 'W')
        tiles.emplace_back(Tile_WallHorizontal);
      else if (tile == 'V')
        tiles.emplace_back(Tile_WallVertical);
      else
        tiles.emplace_back(Tile_Empty);
    }

    auto tilesData = builder.CreateVector(tiles);
    rows.emplace_back(CreateRow(builder, tilesData));
  }

  auto map = builder.CreateVector(rows);
  auto mapData = CreateMapData(builder, map, fd);
  return CreatePacket(builder, MsgType_Map, MsgUnion_MapData, mapData.Union());
}

void Game::setIsDataUpdated(bool value) { this->isDataUpdated.store(value); }

State Game::getSnakeState(const int fd) {
  auto it = this->snakes.find(fd);
  if (it != this->snakes.end() && it->second)
    return it->second->getState();

  return State_Dead;
}

int Game::getHeight() const { return this->height.load(); }

int Game::getWidth() const { return this->width.load(); }

bool Game::getStopFlag() const { return this->stopFlag.load(); }

bool Game::getIsDataUpdated() const { return this->isDataUpdated.load(); }

void Game::printField() {
  std::lock_guard<std::mutex> lock(this->readableFieldMutex);

  printf("\n\n");
  for (size_t i = 0; i < readableField.size(); i++)
    printf("%3zu:%s\n", i, readableField[i].c_str());
  printf("\n\n");
}

bool has_invalid_chars(const std::string& line) {
  for (char c : line) {
    if (c != FLOOR_SYMBOL && c != 'W' && c != 'V' && c != 'F' && c != 'H' && c != 'B' && c != 'T')
      return true;
  }
  return false;
}