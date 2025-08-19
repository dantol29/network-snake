#include "Snake.hpp"
#include "Game.hpp"

Snake::Snake(int height, int width, Game *game) : gameHeight(height), gameWidth(width), game(game)
{
    this->tail = (struct snake *)malloc(sizeof(struct snake));
    if (!this->tail)
        onerror("Memory error");

    this->tail->x = 2;
    this->tail->y = 3;
    this->tail->next = NULL;
    this->tail->prev = NULL;
    this->direction = UP;
}

Snake::~Snake()
{
    if (!this->tail)
        return;

    struct snake *prev = this->tail;
    struct snake *tmp;

    while (prev)
    {
        tmp = prev;
        prev = prev->prev;
        free(tmp);
    }
}

void Snake::moveSnake()
{
    if (!this->tail)
        return;

    int oldY = this->tail->y;
    int oldX = this->tail->x;
    struct snake *currentPart = this->tail;
    while (currentPart)
    {
        if (currentPart->prev)
        {
            currentPart->x = currentPart->prev->x;
            currentPart->y = currentPart->prev->y;
            this->game->field[currentPart->y][currentPart->x] = 'B'; // TODO: maybe possibe to reduce writes
        }
        else
            this->moveHead(currentPart, oldX, oldY);

        currentPart = currentPart->prev;
    }
}

void Snake::moveHead(struct snake *head, int oldX, int oldY)
{
    switch (this->direction)
    {
    case UP:
        if (head->y > 0)
            head->y -= 1;
        else
            head->y = this->gameHeight - 1;
        break;
    case DOWN:
        if (head->y < this->gameHeight - 1)
            head->y += 1;
        else
            head->y = 0;
        break;
    case LEFT:
        if (head->x > 0)
            head->x -= 1;
        else
            head->x = this->gameWidth - 1;
        break;
    case RIGHT:
        if (head->x < this->gameWidth - 1)
            head->x += 1;
        else
            head->x = 0;
    }

    if (this->game->field[head->y][head->x] == 'B')
        onerror("GAME OVER!");

    if (this->game->field[head->y][head->x] == 'F')
    {
        this->growSnake(oldX, oldY);
        this->game->decreaseFood();
        this->game->field[oldY][oldX] = 'B';
    }
    else
        this->game->field[oldY][oldX] = FLOOR_SYMBOL;

    this->game->field[head->y][head->x] = 'H';
}

void Snake::setDirection(enum direction newDirection)
{
    this->direction = newDirection;
}

void Snake::growSnake(int oldX, int oldY)
{
    struct snake *newTail = (struct snake *)malloc(sizeof(struct snake));
    newTail->next = NULL;
    newTail->x = oldX;
    newTail->y = oldY;
    newTail->prev = this->tail;
    this->tail->next = newTail;

    this->tail = newTail;
}
