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
    Game(const int height, const int width);
    ~Game();

    void start();
    void stop();

    void decreaseFood();
    void addSnake(const int fd);
    void addDeadSnake(const int fd);
    void setSnakeDirection(const int fd, const int dir);
    void setIsDataUpdated(bool value);
    std::string fieldToString();

    struct snake getSnakeHead(const int fd) const;
    int getHeight() const;
    int getWidth() const;
    bool getStopFlag() const;
    bool getIsDataUpdated() const;

private:
    int foodCount;
    int maxSnakeCount;
    std::chrono::steady_clock::time_point now;

    // Used by another thread
    std::atomic<int> height;
    std::atomic<int> width;
    std::atomic<bool> stopFlag;
    std::atomic<bool> isDataUpdated;
    std::mutex deadSnakesMutex;
    std::vector<int> deadSnakes;
    std::mutex snakesMutex;
    std::vector<Snake *> snakes;
    std::mutex gameFieldMutex;
    std::vector<std::string> gameField;

    void spawnFood();
    void moveSnakes();
    void removeSnake(const int fd);
    void removeDeadSnakes();
    void increaseGameField();
    void printField() const;
};

#endif
