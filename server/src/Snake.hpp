#ifndef SNAKE_HPP
#define SNAKE_HPP

#include "../includes/nibbler.hpp"
#include "Game.hpp"

class Game;

class Snake {
public:
  Snake(Game* game);
  ~Snake();

  void moveSnake(std::vector<std::string>* gameField);
  void cleanup(std::vector<std::string>* gameField);
  void setDirection(const int newDir);

  State getState() const;
  t_coordinates getHead() const;
  std::list<t_coordinates> getBody() const;

private:
  Game* game;
  std::list<t_coordinates> body;
  enum e_direction direction;
  State state;

  t_coordinates moveHead(int currentX, int currentY, std::vector<std::string>* gameField);
};

#endif
