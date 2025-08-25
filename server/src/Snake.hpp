#ifndef SNAKE_HPP
#define SNAKE_HPP

#include "../includes/nibbler.hpp"
#include "Game.hpp"

class Game;

class Snake
{
public:
    Snake(const int height, const int width, const int fd, Game *game);
    ~Snake();

    void moveSnake(std::vector<std::string> &gameField);
    void cleanSnakeFromField(std::vector<std::string> &gameField);
    void setDirection(const int newDir);
    void updateGameSize(const int height, const int width);

    int getHeadX() const;
    int getHeadY() const;
    int getFd() const;

private:
    Game *game;
    const int fd;
    struct snake *tail;
    direction direction;
    int gameHeight;
    int gameWidth;

    // Used by server thread
    std::atomic<int> headX;
    std::atomic<int> headY;

    void growSnake(const int oldX, const int oldY);
    void moveHead(struct snake *head, const int oldX, const int oldY, std::vector<std::string> &gameField);
};

#endif
