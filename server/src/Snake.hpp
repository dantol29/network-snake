#ifndef SNAKE_HPP
#define SNAKE_HPP

#include "../includes/nibbler.hpp"
#include "Game.hpp"

class Game;

class Snake {
public:
  Snake(Game* game, int fd);
  ~Snake();

  void moveSnake(std::vector<std::string>* gameField);
  void cleanup(std::vector<std::string>* gameField);
  void setDirection(const int newDir);

  bool getIsDead() const;
  t_coordinates getHead() const;

private:
  Game *game;
  std::list<t_coordinates> body;
  enum e_direction direction;
  const int fd;
  bool isDead;
  bool isActive;

  t_coordinates moveHead(int currentX, int currentY, std::vector<std::string> *gameField);
};

#endif
