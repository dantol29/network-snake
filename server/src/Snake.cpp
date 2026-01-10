#include "Snake.hpp"
#include "Game.hpp"

Snake::Snake(Game* game) : game(game), direction(UP), isDirectionSet(false), state(State_Idle), score(0) {
  t_coordinates c;

  c.x = game->getWidth() / 2;
  c.y = game->getHeight() / 2;
  body.push_front(c);
  c.y = game->getHeight() / 2 + 1;
  body.push_back(c);
  c.y = game->getHeight() / 2 + 2;
  body.push_back(c);
  c.y = game->getHeight() / 2 + 3;
  body.push_back(c);
}

Snake::~Snake() { std::cout << "Snake destructor" << std::endl; }

void Snake::moveSnake(std::vector<std::string>* gameField) {
  auto currentHead = body.front();
  auto currentTail = body.back();

  if (state == State_Alive) {
    currentHead = moveHead(currentHead.x, currentHead.y, gameField);
    if (state == State_Dead)
      return;

    body.push_front({currentHead.x, currentHead.y});

    if ((*gameField)[currentHead.y][currentHead.x] == FOOD_TILE) {
      game->removeFood(currentHead.x, currentHead.y);
      score += 1;
    } else {
      (*gameField)[currentTail.y][currentTail.x] = FLOOR_TILE;
      body.pop_back();
      currentTail = body.back();
    }
  }

  for (const auto& segment : body)
    (*gameField)[segment.y][segment.x] = BODY_TILE;

  (*gameField)[currentHead.y][currentHead.x] = HEAD_TILE;
  (*gameField)[currentTail.y][currentTail.x] = TAIL_TILE;

  isDirectionSet = false;
}

t_coordinates Snake::moveHead(int currentX, int currentY, std::vector<std::string>* gameField) {
  switch (direction) {
  case UP:
    if (currentY > 0)
      currentY -= 1;
    else
      state = State_Dead;
    break;
  case DOWN:
    if (currentY < game->getHeight() - 1)
      currentY += 1;
    else
      state = State_Dead;
    break;
  case LEFT:
    if (currentX > 0)
      currentX -= 1;
    else
      state = State_Dead;
    break;
  case RIGHT:
    if (currentX < game->getWidth() - 1)
      currentX += 1;
    else
      state = State_Dead;
  }

  char tile = (*gameField)[currentY][currentX];
  if (tile == BODY_TILE || tile == HEAD_TILE || tile == WALL_HORIZ_TILE || tile == WALL_VERTI_TILE)
    state = State_Dead;

  return {currentX, currentY};
}

void Snake::setDirection(const int newDir) {
  if (state == State_Idle)
    state = State_Alive;

  if (isDirectionSet)
	return;

  enum e_direction dir = (enum e_direction)newDir;
  if ((dir == UP || dir == DOWN) && (direction == DOWN || direction == UP))
    return;
  if ((dir == RIGHT || dir == LEFT) && (direction == RIGHT || direction == LEFT))
    return;

  direction = dir;
  isDirectionSet = true;
  std::cout << "Received " << std::endl;
}

void Snake::cleanup(std::vector<std::string>* gameField) {
  for (const auto& segment : body)
    (*gameField)[segment.y][segment.x] = FLOOR_TILE;
}

int Snake::getScore() const { return score; }

t_coordinates Snake::getHead() const { return body.front(); }

State Snake::getState() const { return state; }

std::list<t_coordinates> Snake::getBody() const { return body; }