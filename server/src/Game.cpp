#include "Game.hpp"
#include <chrono>

#define MAX_FOOD_SPAWN_TRIES 50

using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;

TimePoint lastMoveTime = Clock::now();
TimePoint lastEatTime = Clock::now();

Game::Game(const int height, const int width) : height(height), width(width), stopFlag(false), snakeCount(0)
{
    for (int i = 0; i < height; i++)
    {
        std::string row(width, FLOOR_SYMBOL);
        this->readableField.push_back(row);
        this->writableField.push_back(row);
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
            this->moveSnakes();
            this->spawnFood();

            this->updateReadableField();
            this->setIsDataUpdated(true);
            nextMoveTime = now + std::chrono::milliseconds(SNAKE_SPEED);
        }

        std::this_thread::sleep_until(nextMoveTime);
    }
}

void Game::spawnFood()
{
    if (!(this->snakeCount.load() * 2 > this->foodCount))
        return;

    for (int i = 0; i < MAX_FOOD_SPAWN_TRIES; i++)
    {
        int r1 = rand();
        int r2 = rand();
        int x = r1 % (this->width - 1);
        int y = r2 % (this->height - 1);

        if (this->writableField[y][x] == FLOOR_SYMBOL)
        {
            this->writableField[y][x] = 'F';
            ++this->foodCount;
            return;
        }
    }
}

void Game::moveSnakes()
{
    std::lock_guard<std::mutex> lock(this->snakesMutex);
    for (auto it = this->snakes.begin(); it != this->snakes.end();)
    {
        it->second->moveSnake(&this->writableField);
        if (it->second->getIsDead())
        {
            it->second->cleanup(&writableField);
            delete it->second;
            it = snakes.erase(it);
            --this->snakeCount;
        }
        else
            ++it;
    }
}

void Game::addSnake(const int clientFd)
{
    std::lock_guard<std::mutex> lock1(snakesMutex);
    Snake *newSnake = new Snake(this, clientFd);
    this->snakes[clientFd] = newSnake;

    ++this->snakeCount;
}

void Game::removeSnake(int fd)
{
    std::lock_guard<std::mutex> lock(this->snakesMutex);

    auto snake = this->snakes.find(fd);
    if (snake != this->snakes.end() && snake->second)
    {
        snake->second->cleanup(&this->writableField);
        delete snake->second;
        this->snakes.erase(snake);
        --this->snakeCount;
    }
}

void Game::updateReadableField()
{
    std::lock_guard<std::mutex> lock(this->readableFieldMutex);

    this->readableField = this->writableField;
}

void Game::updateSnakeDirection(const int fd, const int dir)
{
    std::lock_guard<std::mutex> lock(this->snakesMutex);

    auto it = this->snakes.find(fd);
    if (it != this->snakes.end() && it->second)
        this->snakes[fd]->setDirection(dir);
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
    std::lock_guard<std::mutex> lock(this->readableFieldMutex);

    std::string data;

    for (int i = 0; i < readableField.size(); i++)
        data += readableField[i];

    return data;
}

void Game::printField()
{
    std::lock_guard<std::mutex> lock(this->readableFieldMutex);

    printf("\n\n");
    for (int i = 0; i < readableField.size(); i++)
        printf("%3d:%s\n", i, readableField[i].c_str());
    printf("\n\n");
}
