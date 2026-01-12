#ifndef SNAKE_HPP
#define SNAKE_HPP

#include "../includes/nibbler.hpp"
#include "Game.hpp"

class Game;

class Snake {
public:
  Snake(Game* game);
  Snake(const Snake& obj) = delete;
  Snake& operator=(const Snake& obj) = delete;
  Snake(Snake&& obj) = delete;
  Snake& operator=(Snake&& obj) = delete;
  ~Snake();

  void moveSnake(std::vector<std::string>* gameField);
  void cleanup(std::vector<std::string>* gameField);
  void setDirection(const int newDir);

  int getScore() const;
  State getState() const;
  t_coordinates getHead() const;
  std::list<t_coordinates> getBody() const;

private:
  Game* game;
  std::list<t_coordinates> body;
  enum e_direction direction;
  bool isDirectionSet;
  State state;
  int score;

  t_coordinates moveHead(int currentX, int currentY, std::vector<std::string>* gameField);
};

#endif
