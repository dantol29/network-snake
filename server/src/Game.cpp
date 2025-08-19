#include "Game.hpp"
#include <chrono>

#define MAX_FOOD_SPAWN_TRIES 50

using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;

TimePoint lastMoveTime = Clock::now();
TimePoint lastEatTime = Clock::now();

Game::Game(int width, int height) : width(width), height(height), fieldSize(width * height)
{
    for (int i = 0; i < height; i++)
    {
        std::string row(width, FLOOR_SYMBOL);
        this->field.push_back(row);
    }

    this->foodCount = 0;
    srand(time(NULL)); // init random generator
}

void Game::gameLoop()
{
    while (1)
    {
        auto now = Clock::now();
        bool canMove = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMoveTime).count() >= 300;
        bool canEat = std::chrono::duration_cast<std::chrono::seconds>(now - lastEatTime).count() >= 3;

        if (canMove)
        {
            this->snake->moveSnake();
            this->printField();
            lastMoveTime = now;
        }

        if (canEat)
        {
            this->spawnFood();
            lastEatTime = now;
        }
    }
}

void Game::spawnFood()
{
    if (foodCount > 1)
        return;

    for (int i = 0; i < MAX_FOOD_SPAWN_TRIES; i++)
    {
        int r1 = rand();
        int r2 = rand();
        int x = r1 % this->width;
        int y = r2 % this->height;

        if (this->field[y][x] == FLOOR_SYMBOL)
        {
            std::cout << "FOOD!" << std::endl;
            this->field[y][x] = 'F';
            ++foodCount;
            return;
        }
    }
}

/*
    UTILS
*/

void Game::decreaseFood()
{
    if (this->foodCount > 0)
        this->foodCount--;
}

void Game::addSnake(Snake *snake)
{
    this->snake = snake;
}

int Game::getFieldSize() const
{
    return this->fieldSize;
}

std::string Game::fieldToString() const
{
    std::string data;

    for (int i = 0; i < this->field.size(); i++)
        data += this->field[i];

    return data;
}

void Game::printField() const
{
    printf("\n\n");
    for (int i = 0; i < this->field.size(); i++)
        printf("%3d:%s\n", i, this->field[i].c_str());
    printf("\n\n");
}
