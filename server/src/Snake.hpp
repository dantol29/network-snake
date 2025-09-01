#ifndef SNAKE_HPP
#define SNAKE_HPP

#include "../includes/nibbler.hpp"
#include "Game.hpp"

class Game;

class Snake
{
public:
    Snake(Game *game, int fd);
    ~Snake();

    void moveSnake(std::vector<std::string> *gameField);
    void cleanup(std::vector<std::string> *gameField);
    void setDirection(const int newDir);

    bool getIsDead() const;
    int getFd() const;
    struct coordinates getHead() const;

private:
    Game *game;
    std::list<struct coordinates> body;
    enum e_direction direction;
    const int fd;
    bool isDead;

    struct coordinates moveHead(int currentX, int currentY, std::vector<std::string> *gameField);
};

#endif
