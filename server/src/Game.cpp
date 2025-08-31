#include "Game.hpp"
#include <chrono>

#define MAX_FOOD_SPAWN_TRIES 50

using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;

TimePoint lastMoveTime = Clock::now();
TimePoint lastEatTime = Clock::now();

Game::Game(const int height, const int width) : height(height), width(width), stopFlag(false)
{
    for (int i = 0; i < height; i++)
    {
        std::string row(width, FLOOR_SYMBOL);
        this->gameField.push_back(row);
    }

    this->foodCount = 0;
    this->maxSnakeCount = 0;

    srand(time(NULL)); // init random generator
}

Game::~Game()
{
    std::cout << "Game destr called" << std::endl;
}

void Game::stop()
{
    this->stopFlag.store(true);
}

void Game::start()
{
    while (1)
    {
        this->now = Clock::now();
        bool move = std::chrono::duration_cast<std::chrono::milliseconds>(this->now - lastMoveTime).count() >= SNAKE_SPEED;

        if (move)
            this->moveSnakes();

        if (this->snakes.size() * 2 <= this->foodCount)
            this->spawnFood();

        if (stopFlag.load())
            return;
    }
}

void Game::spawnFood()
{
    std::lock_guard<std::mutex> lock(gameFieldMutex);

    for (int i = 0; i < MAX_FOOD_SPAWN_TRIES; i++)
    {
        int r1 = rand();
        int r2 = rand();
        int x = r1 % (this->width - 1);
        int y = r2 % (this->height - 1);

        if (this->gameField[y][x] == FLOOR_SYMBOL)
        {
            this->gameField[y][x] = 'F';
            lastEatTime = this->now;
            ++this->foodCount;
            return;
        }
    }

    this->setIsDataUpdated(true);
}

void Game::moveSnakes()
{
    std::lock_guard<std::mutex> lock1(this->gameFieldMutex);
    std::lock_guard<std::mutex> lock2(this->snakesMutex);

    for (auto it = this->snakes.begin(); it != this->snakes.end(); it++)
        (*it).second->moveSnake(this->gameField);

    this->removeDeadSnakes();
    this->setIsDataUpdated(true);

    lastMoveTime = this->now;
}

void Game::addSnake(const int clientFd, in_addr_t clientAddr)
{
    std::lock_guard<std::mutex> lock1(snakesMutex);
    std::lock_guard<std::mutex> lock2(gameFieldMutex);

    Snake *newSnake = new Snake(this, this->height, this->width, clientFd, clientAddr);
    this->snakes[clientAddr] = newSnake;

    if (this->snakes.size() > this->maxSnakeCount)
    {
        this->maxSnakeCount = this->snakes.size();
        this->increaseGameField();
    }
}

void Game::addDeadSnake(const int fd)
{
    std::lock_guard<std::mutex> lock3(this->deadSnakesMutex);
    this->deadSnakes.push_back(fd);
}

void Game::removeSnake(const int fd)
{
    for (auto iter = this->snakes.begin(); iter != this->snakes.end(); iter++)
    {
        if ((*iter).second->getFd() == fd)
        {
            (*iter).second->cleanSnakeFromField(this->gameField);
            this->snakes.erase(iter);
            return;
        }
    }
}

void Game::removeDeadSnakes()
{
    std::lock_guard<std::mutex> lock(this->deadSnakesMutex);

    for (auto it = this->deadSnakes.begin(); it != this->deadSnakes.end(); it++)
        this->removeSnake(*it);
    this->deadSnakes.clear();
}

void Game::updateSnakeDirection(in_addr_t clientAddr, const int dir)
{
    std::lock_guard<std::mutex> lock(this->snakesMutex);
    this->snakes[clientAddr]->setDirection(dir);
}

void Game::increaseGameField()
{
    int currentHeight = this->height.load();
    int currentWidth = this->width.load();
    int newHeight = static_cast<int>(currentHeight * 1.10 + 0.5);
    int newWidth = static_cast<int>(currentWidth * 1.10 + 0.5);

    this->height.store(newHeight);
    this->width.store(newWidth);

    for (auto iter = this->snakes.begin(); iter != this->snakes.end(); ++iter)
        (*iter).second->updateGameSize(newHeight, newWidth);

    this->gameField.resize(newHeight);
    for (int i = 0; i < newHeight; i++)
        this->gameField[i].resize(newWidth, '.');
}

void Game::decreaseFood()
{
    if (this->foodCount > 0)
        this->foodCount--;
}

void Game::setIsDataUpdated(bool value)
{
    this->isDataUpdated.store(value);
}

/// GETTERS

struct snake Game::getSnakeHead(const int fd) const
{
    struct snake head = {0};

    for (auto iter = this->snakes.begin(); iter != this->snakes.end(); iter++)
    {
        if ((*iter).second->getFd() == fd)
        {
            head.x = (*iter).second->getHeadX();
            head.y = (*iter).second->getHeadY();
            break;
        }
    }

    return head;
}

int Game::getHeight() const
{
    return this->height.load();
}

int Game::getWidth() const
{
    return this->width.load();
}

bool Game::getStopFlag() const
{
    return this->stopFlag.load();
}

bool Game::getIsDataUpdated() const
{
    return this->isDataUpdated.load();
}

/// UTILS

std::string Game::fieldToString()
{
    std::lock_guard<std::mutex> lock(this->gameFieldMutex);
    std::string data;

    for (int i = 0; i < this->gameField.size(); i++)
        data += this->gameField[i];

    return data;
}

void Game::printField() const
{
    printf("\n\n");
    for (int i = 0; i < this->gameField.size(); i++)
        printf("%3d:%s\n", i, this->gameField[i].c_str());
    printf("\n\n");
}
