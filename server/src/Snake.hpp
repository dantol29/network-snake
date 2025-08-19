#ifndef SNAKE_HPP
#define SNAKE_HPP

#include "../includes/nibbler.hpp"
#include "Game.hpp"

struct snake
{
    int x;
    int y;
    snake *next;
    snake *prev;
};

enum direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Snake
{
public:
    Snake(int height, int width, Game *game);
    ~Snake();

    void moveSnake();
    void setDirection(enum direction newDirection);

private:
    Game *game;
    struct snake *tail = nullptr;
    direction direction;
    int gameHeight;
    int gameWidth;

    void growSnake(int oldX, int oldY);
    void moveHead(struct snake *head, int oldX, int oldY);
};

#endif
