#include "Snake.hpp"
#include "Game.hpp"

Snake::Snake(Game* game, int fd) : game(game), fd(fd) {
  t_coordinates c;
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
  this->state = State_Idle;
}

Snake::~Snake() { std::cout << "Snake destructor called" << std::endl; }

void Snake::moveSnake(std::vector<std::string>* gameField) {
  auto currentHead = body.front();
  auto currentTail = body.back();

  if (this->state == State_Alive) {
    currentHead = moveHead(currentHead.x, currentHead.y, gameField);
    body.push_front({currentHead.x, currentHead.y});

    if ((*gameField)[currentHead.y][currentHead.x] == 'F')
      this->game->decreaseFood();
    else {
      (*gameField)[currentTail.y][currentTail.x] = FLOOR_SYMBOL;
      body.pop_back();
      currentTail = body.back();
    }
  }

  for (const auto& segment : body)
    (*gameField)[segment.y][segment.x] = 'B';

  (*gameField)[currentHead.y][currentHead.x] = 'H';
  (*gameField)[currentTail.y][currentTail.x] = 'T';
}

t_coordinates Snake::moveHead(int currentX, int currentY, std::vector<std::string>* gameField) {
  switch (this->direction) {
  case UP:
    if (currentY > 0)
      currentY -= 1;
    else
      this->state = State_Dead;
    break;
  case DOWN:
    if (currentY < this->game->getHeight() - 1)
      currentY += 1;
    else
      this->state = State_Dead;
    break;
  case LEFT:
    if (currentX > 0)
      currentX -= 1;
    else
      this->state = State_Dead;
    break;
  case RIGHT:
    if (currentX < this->game->getWidth() - 1)
      currentX += 1;
    else
      this->state = State_Dead;
  }

  char tile = (*gameField)[currentY][currentX];
  if (tile == 'B' || tile == 'H' || tile == 'W')
    this->state = State_Dead;

  return {currentX, currentY};
}

void Snake::setDirection(const int newDir) {
  this->state = State_Alive;

  enum e_direction dir = (enum e_direction)newDir;
  if ((dir == UP || dir == DOWN) && (this->direction == DOWN || this->direction == UP))
    return;
  if ((dir == RIGHT || dir == LEFT) && (this->direction == RIGHT || this->direction == LEFT))
    return;

  this->direction = dir;
}

void Snake::cleanup(std::vector<std::string>* gameField) {
  for (const auto& segment : body)
    (*gameField)[segment.y][segment.x] = FLOOR_SYMBOL;
}

t_coordinates Snake::getHead() const { return this->body.front(); }

State Snake::getState() const { return this->state; }

std::list<t_coordinates> Snake::getBody() const { return body; }