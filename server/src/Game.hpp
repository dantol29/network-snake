#ifndef GAME_HPP
#define GAME_HPP

#include "../includes/nibbler.hpp"
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
    void removeSnake(const int fd);
    void updateSnakeDirection(const int fd, const int dir);
    void setIsDataUpdated(bool value);
    std::string fieldToString();
    void printField() const;

    struct coordinates getSnakeHead(const int fd);
    int getHeight() const;
    int getWidth() const;
    bool getStopFlag() const;
    bool getIsDataUpdated() const;

private:
    int foodCount;

    // Used by another thread
    std::atomic<int> height;
    std::atomic<int> width;
    std::atomic<bool> stopFlag;
    std::atomic<bool> isDataUpdated;
    std::mutex snakesMutex;
    std::unordered_map<int, Snake *> snakes;

    std::vector<std::string> gameFieldA;
    std::vector<std::string> gameFieldB;
    std::atomic<std::vector<std::string> *> readableField{&gameFieldA};
    std::vector<std::string> *writableField = &gameFieldB;

    void spawnFood();
    void moveSnakes();
    void swapBuffers();
};

#endif
