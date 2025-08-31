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

    srand(time(NULL)); // init random generator
}

Game::~Game()
{
    std::cout << "Game destr called" << std::endl;

    for (auto it = this->snakes.begin(); it != this->snakes.end(); it++)
        delete it->second;
}

void Game::stop()
{
    this->stopFlag.store(true);
}

void Game::start()
{
    auto nextMoveTime = Clock::now() + std::chrono::milliseconds(SNAKE_SPEED);

    while (!stopFlag.load())
    {
        auto now = Clock::now();
        if (now >= nextMoveTime)
        {
            std::lock_guard<std::mutex> lock1(this->snakesMutex);
            std::lock_guard<std::mutex> lock2(this->gameFieldMutex);

            this->moveSnakes();
            this->spawnFood();
            nextMoveTime = now + std::chrono::milliseconds(SNAKE_SPEED);
        }

        std::this_thread::sleep_until(nextMoveTime);
    }
}

void Game::spawnFood()
{
    if (!(this->snakes.size() * 2 > this->foodCount))
        return;

    for (int i = 0; i < MAX_FOOD_SPAWN_TRIES; i++)
    {
        int r1 = rand();
        int r2 = rand();
        int x = r1 % (this->width - 1);
        int y = r2 % (this->height - 1);

        if (this->gameField[y][x] == FLOOR_SYMBOL)
        {
            this->gameField[y][x] = 'F';
            ++this->foodCount;
            return;
        }
    }

    this->setIsDataUpdated(true);
}

void Game::moveSnakes()
{
    for (auto it = this->snakes.begin(); it != this->snakes.end(); it++)
        it->second->moveSnake(this->gameField);

    this->removeDeadSnakes();
    this->setIsDataUpdated(true);

    lastMoveTime = Clock::now();
}

void Game::addSnake(const int clientFd)
{
    std::lock_guard<std::mutex> lock1(snakesMutex);
    Snake *newSnake = new Snake(this, clientFd);
    this->snakes[clientFd] = newSnake;
}

void Game::addDeadSnake(const int fd)
{
    std::lock_guard<std::mutex> lock3(this->deadSnakesMutex);
    this->deadSnakes.push_back(fd);
}

void Game::removeDeadSnakes()
{
    std::lock_guard<std::mutex> lock(this->deadSnakesMutex);

    for (auto it = this->deadSnakes.begin(); it != this->deadSnakes.end(); it++)
    {
        auto snake = this->snakes.find(*it);
        if (snake != this->snakes.end() && snake->second)
        {
            snake->second->cleanup(this->gameField);
            delete snake->second;
            this->snakes.erase(snake);
        }
    }
    this->deadSnakes.clear();
}

void Game::updateSnakeDirection(const int fd, const int dir)
{
    std::lock_guard<std::mutex> lock(this->snakesMutex);
    this->snakes[fd]->setDirection(dir);
}

void Game::increaseGameField()
{
    int currentHeight = this->height.load();
    int currentWidth = this->width.load();
    int newHeight = static_cast<int>(currentHeight * 1.10 + 0.5);
    int newWidth = static_cast<int>(currentWidth * 1.10 + 0.5);

    this->height.store(newHeight);
    this->width.store(newWidth);

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

struct coordinates Game::getSnakeHead(const int fd)
{
    std::lock_guard<std::mutex> lock(snakesMutex);

    struct coordinates head = {0};
    auto it = this->snakes.find(fd);
    if (it != this->snakes.end() && it->second)
        head = it->second->getHead();

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
