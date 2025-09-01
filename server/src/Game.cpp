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
        this->gameFieldA.push_back(row);
        this->gameFieldB.push_back(row);
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

            this->swapBuffers();
            this->setIsDataUpdated(true);
            nextMoveTime = now + std::chrono::milliseconds(SNAKE_SPEED);
        }

        std::this_thread::sleep_until(nextMoveTime);
    }
}

void Game::spawnFood()
{
    {
        std::lock_guard<std::mutex> lock(this->snakesMutex);
        if (!(this->snakes.size() * 2 > this->foodCount))
            return;
    }

    for (int i = 0; i < MAX_FOOD_SPAWN_TRIES; i++)
    {
        int r1 = rand();
        int r2 = rand();
        int x = r1 % (this->width - 1);
        int y = r2 % (this->height - 1);

        if ((*this->writableField)[y][x] == FLOOR_SYMBOL)
        {
            (*this->writableField)[y][x] = 'F';
            ++this->foodCount;
            return;
        }
    }

    this->setIsDataUpdated(true);
}

void Game::moveSnakes()
{
    {
        std::lock_guard<std::mutex> lock(this->snakesMutex);
        for (auto it = this->snakes.begin(); it != this->snakes.end(); it++)
            it->second->moveSnake(this->writableField);
    }

    for (auto it = this->snakes.begin(); it != this->snakes.end(); it++)
    {
        if (it->second->getIsDead())
            this->removeSnake(it->second->getFd());
    }

    lastMoveTime = Clock::now();
}

void Game::addSnake(const int clientFd)
{
    std::lock_guard<std::mutex> lock1(snakesMutex);
    Snake *newSnake = new Snake(this, clientFd);
    this->snakes[clientFd] = newSnake;
}

void Game::removeSnake(int fd)
{
    std::lock_guard<std::mutex> lock(this->snakesMutex);

    auto snake = this->snakes.find(fd);
    if (snake != this->snakes.end() && snake->second)
    {
        snake->second->cleanup(this->writableField);
        delete snake->second;
        this->snakes.erase(snake);
    }
}

void Game::swapBuffers()
{
    auto oldReadable = this->readableField.load();

    this->readableField.store(this->writableField);
    this->writableField = oldReadable;

    *this->writableField = *this->readableField.load();
}

void Game::updateSnakeDirection(const int fd, const int dir)
{
    std::lock_guard<std::mutex> lock(this->snakesMutex);
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
    std::string data;
    const std::vector<std::string> &gameField = *(readableField.load());

    for (int i = 0; i < gameField.size(); i++)
        data += gameField[i];

    return data;
}

void Game::printField() const
{
    const std::vector<std::string> &gameField = *(readableField.load());

    printf("\n\n");
    for (int i = 0; i < gameField.size(); i++)
        printf("%3d:%s\n", i, gameField[i].c_str());
    printf("\n\n");
}
