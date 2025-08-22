#ifndef GAME_HPP
#define GAME_HPP

#include "../includes/nibbler.hpp"
#include <vector>
#include <string>
#include "Snake.hpp"

class Snake;

class Game
{
public:
    Game(int size);
    ~Game() = default;

    void gameLoop();
    void decreaseFood();
    void addSnake(int fd);
    void addDeadSnake(int fd);
    void setSnakeDirection(int fd, int dir);
    int getFieldSize() const;
    std::string fieldToString();

private:
    int foodCount;
    const int fieldSize;
    std::chrono::steady_clock::time_point now;

    // Used by another thread
    std::mutex deadSnakesMutex;
    std::vector<int> deadSnakes;
    std::mutex snakesMutex;
    std::vector<Snake *> snakes;
    std::mutex gameFieldMutex;
    std::vector<std::string> gameField;

    void spawnFood();
    void moveSnakes();
    void removeSnake(int fd);
    void printField() const;
};

#endif
