#ifndef SNAKE_HPP
#define SNAKE_HPP

#include "../includes/nibbler.hpp"
#include "Game.hpp"

class Game;

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
    Snake(int height, int width, int fd, Game *game);
    ~Snake();

    void moveSnake(std::vector<std::string> &gameField);
    void cleanSnakeFromField(std::vector<std::string> &gameField);
    void setDirection(int newDir);
    int getFd() const;

private:
    Game *game;
    struct snake *tail;
    direction direction;
    int gameHeight;
    int gameWidth;
    int fd;

    void growSnake(int oldX, int oldY);
    void moveHead(struct snake *head, int oldX, int oldY, std::vector<std::string> &gameField);
};

#endif
