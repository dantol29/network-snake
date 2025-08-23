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
    Snake(const int height, const int width, const int fd, Game *game);
    ~Snake();

    void moveSnake(std::vector<std::string> &gameField);
    void cleanSnakeFromField(std::vector<std::string> &gameField);
    void setDirection(const int newDir);
    void updateGameSize(const int height, const int width);

    int getFd() const;

private:
    Game *game;
    const int fd;
    struct snake *tail;
    direction direction;
    int gameHeight;
    int gameWidth;

    void growSnake(const int oldX, const int oldY);
    void moveHead(struct snake *head, const int oldX, const int oldY, std::vector<std::string> &gameField);
};

#endif
