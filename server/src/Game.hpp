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
    Game(int height, int width);
    ~Game();

    void start();
    void stop();

    void decreaseFood();
    void addSnake(int fd);
    void addDeadSnake(int fd);
    void setSnakeDirection(int fd, int dir);
    std::string fieldToString();

    bool getStopFlag();
    int getHeight() const;
    int getWidth() const;

private:
    int foodCount;
    const int height;
    const int width;
    std::chrono::steady_clock::time_point now;

    // Used by another thread
    std::atomic<bool> stopFlag;
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
