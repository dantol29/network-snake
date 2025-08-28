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
        bool move = std::chrono::duration_cast<std::chrono::milliseconds>(this->now - lastMoveTime).count() >= 300;
        bool spawnFood = std::chrono::duration_cast<std::chrono::seconds>(this->now - lastEatTime).count() >= 3;

        if (move)
            this->moveSnakes();

        if (spawnFood)
            this->spawnFood();

        if (stopFlag.load())
            return;
    }
}

void Game::spawnFood()
{
    if (this->snakes.size() * 2 <= this->foodCount)
        return;

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
}

void Game::moveSnakes()
{
    std::lock_guard<std::mutex> lock1(this->gameFieldMutex);
    std::lock_guard<std::mutex> lock2(this->snakesMutex);

    for (auto it = this->snakes.begin(); it != this->snakes.end(); it++)
        (*it)->moveSnake(this->gameField);

    this->removeDeadSnakes();

    // this->printField();
    lastMoveTime = this->now;
}

void Game::addSnake(const int clientFd)
{
    std::lock_guard<std::mutex> lock1(snakesMutex);
    std::lock_guard<std::mutex> lock2(gameFieldMutex);

    Snake *newSnake = new Snake(this->height, this->width, clientFd, this);
    this->snakes.push_back(newSnake);

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
        if ((*iter)->getFd() == fd)
        {
            (*iter)->cleanSnakeFromField(this->gameField);
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

void Game::setSnakeDirection(const int fd, const int dir)
{
    std::lock_guard<std::mutex> lock(this->snakesMutex);
    for (auto iter = this->snakes.begin(); iter != this->snakes.end(); iter++)
        if ((*iter)->getFd() == fd)
            return (*iter)->setDirection(dir);
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
        (*iter)->updateGameSize(newHeight, newWidth);

    this->gameField.resize(newHeight);
    for (int i = 0; i < newHeight; i++)
        this->gameField[i].resize(newWidth, '.');
}

void Game::decreaseFood()
{
    if (this->foodCount > 0)
        this->foodCount--;
}

/// GETTERS

struct snake Game::getSnakeHead(const int fd) const
{
    struct snake head = {0};

    for (auto iter = this->snakes.begin(); iter != this->snakes.end(); iter++)
    {
        if ((*iter)->getFd() == fd)
        {
            head.x = (*iter)->getHeadX();
            head.y = (*iter)->getHeadY();
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
