#include "Snake.hpp"
#include "Game.hpp"

Snake::Snake(Game *game, int fd) : game(game), fd(fd) {
  struct coordinates c;
  c.x = this->game->getWidth() / 2;
  c.y = this->game->getHeight() / 2;
  this->body.push_front(c);
  c.y = this->game->getHeight() / 2 + 1;
  this->body.push_back(c);
  c.y = this->game->getHeight() / 2 + 2;
  this->body.push_back(c);
  c.y = this->game->getHeight() / 2 + 3;
  this->body.push_back(c);

  this->direction = UP;
  this->isDead = false;
}

Snake::~Snake() { std::cout << "Snake destructor called" << std::endl; }

void Snake::moveSnake(std::vector<std::string> *gameField) {
  auto currentHead = body.front();
  auto newCoord = moveHead(currentHead.x, currentHead.y, gameField);
  body.push_front({newCoord.x, newCoord.y});

  if ((*gameField)[newCoord.y][newCoord.x] == 'F')
    this->game->decreaseFood();
  else {
    auto tail = body.back();
    (*gameField)[tail.y][tail.x] = FLOOR_SYMBOL;
    body.pop_back();
  }

  for (const auto &segment : body)
    (*gameField)[segment.y][segment.x] = 'B';

  (*gameField)[newCoord.y][newCoord.x] = 'H';
}

struct coordinates Snake::moveHead(int currentX, int currentY,
                                   std::vector<std::string> *gameField) {
  switch (this->direction) {
  case UP:
    if (currentY > 0)
      currentY -= 1;
    else
      this->isDead = true;
    break;
  case DOWN:
    if (currentY < this->game->getHeight() - 1)
      currentY += 1;
    else
      this->isDead = true;
    break;
  case LEFT:
    if (currentX > 0)
      currentX -= 1;
    else
      this->isDead = true;
    break;
  case RIGHT:
    if (currentX < this->game->getWidth() - 1)
      currentX += 1;
    else
      this->isDead = true;
  }

  if ((*gameField)[currentY][currentX] == 'B' ||
      (*gameField)[currentY][currentX] == 'H')
    this->isDead = true;

  return {currentX, currentY};
}

void Snake::setDirection(const int newDir) {
  enum e_direction dir = (enum e_direction)newDir;
  if ((dir == UP || dir == DOWN) &&
      (this->direction == DOWN || this->direction == UP))
    return;
  if ((dir == RIGHT || dir == LEFT) &&
      (this->direction == RIGHT || this->direction == LEFT))
    return;

  this->direction = dir;
}

void Snake::cleanup(std::vector<std::string> *gameField) {
  for (const auto &segment : body)
    (*gameField)[segment.y][segment.x] = FLOOR_SYMBOL;
}

struct coordinates Snake::getHead() const { return this->body.front(); }

bool Snake::getIsDead() const { return this->isDead; }
