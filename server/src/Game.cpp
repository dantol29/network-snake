#include "Game.hpp"
#include <chrono>

#define MAX_FOOD_SPAWN_TRIES 50

using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;

TimePoint lastMoveTime = Clock::now();
TimePoint lastEatTime = Clock::now();

Game::Game(int size) : fieldSize(size)
{
    for (int i = 0; i < size; i++)
    {
        std::string row(size, FLOOR_SYMBOL);
        this->gameField.push_back(row);
    }

    this->foodCount = 0;
    srand(time(NULL)); // init random generator
}

void Game::gameLoop()
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
    }
}

void Game::spawnFood()
{
    if (foodCount > 1)
        return;

    std::lock_guard<std::mutex> lock(gameFieldMutex);

    for (int i = 0; i < MAX_FOOD_SPAWN_TRIES; i++)
    {
        int r1 = rand();
        int r2 = rand();
        int x = r1 % this->fieldSize;
        int y = r2 % this->fieldSize;

        if (this->gameField[y][x] == FLOOR_SYMBOL)
        {
            this->gameField[y][x] = 'F';
            lastEatTime = this->now;
            ++foodCount;
            return;
        }
    }
}

void Game::decreaseFood()
{
    if (this->foodCount > 0)
        this->foodCount--;
}

void Game::moveSnakes()
{
    std::lock_guard<std::mutex> lock1(this->gameFieldMutex);
    std::lock_guard<std::mutex> lock2(this->snakesMutex);

    for (auto it = this->snakes.begin(); it != this->snakes.end(); it++)
        (*it)->moveSnake(this->gameField);

    // this->printField();
    lastMoveTime = this->now;
}

void Game::setSnakeDirection(int fd, int dir)
{
    std::lock_guard<std::mutex> lock(this->snakesMutex);
    for (auto iter = this->snakes.begin(); iter != this->snakes.end(); iter++)
        if ((*iter)->getFd() == fd)
            return (*iter)->setDirection(dir);
}

void Game::addSnake(int clientFd)
{
    std::lock_guard<std::mutex> lock(snakesMutex);

    Snake *newSnake = new Snake(this->getFieldSize(), this->getFieldSize(), clientFd, this);
    this->snakes.push_back(newSnake);
}

void Game::removeSnake(int fd)
{
    std::lock_guard<std::mutex> lock1(snakesMutex);
    std::lock_guard<std::mutex> lock2(gameFieldMutex);

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

int Game::getFieldSize() const
{
    return this->fieldSize;
}
