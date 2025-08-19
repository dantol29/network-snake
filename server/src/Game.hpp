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
    ~Game() = default;

    void gameLoop();
    void decreaseFood();
    void addSnake(Snake *snake);
    int getFieldSize() const;
    std::string fieldToString() const;

    std::vector<std::string> field;

private:
    Snake *snake;
    int width;
    int height;
    int foodCount;
    const int fieldSize;

    void spawnFood();
    void printField() const;
};

#endif
