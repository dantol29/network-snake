#include "Snake.hpp"
#include "Game.hpp"

Snake::Snake(Game *game, int height, int width, int fd, in_addr_t clientAddr) : gameHeight(height), gameWidth(width), fd(fd), clientAddr(clientAddr), game(game), headX(height / 2), headY(width / 2)
{
    this->tail = (struct snake *)malloc(sizeof(struct snake));
    if (!this->tail)
        onerror("Memory error");

    this->tail->x = height / 2;
    this->tail->y = width / 2;
    this->tail->next = NULL;
    this->tail->prev = NULL;
    this->direction = UP;
}

// TODO: migrate to C++ linked list
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

    std::cout << "Snake destructor called" << std::endl;
}

void Snake::moveSnake(std::vector<std::string> &gameField)
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
            gameField[currentPart->y][currentPart->x] = 'B'; // TODO: maybe possibe to reduce writes
        }
        else
            this->moveHead(currentPart, oldX, oldY, gameField);

        currentPart = currentPart->prev;
    }
}

void Snake::moveHead(struct snake *head, int oldX, int oldY, std::vector<std::string> &gameField)
{
    bool isDead = false;

    switch (this->direction)
    {
    case UP:
        if (head->y > 0)
            head->y -= 1;
        else
            isDead = true;
        break;
    case DOWN:
        if (head->y < this->gameHeight - 1)
            head->y += 1;
        else
            isDead = true;
        break;
    case LEFT:
        if (head->x > 0)
            head->x -= 1;
        else
            isDead = true;
        break;
    case RIGHT:
        if (head->x < this->gameWidth - 1)
            head->x += 1;
        else
            isDead = true;
    }

    if (gameField[head->y][head->x] == 'F')
    {
        this->growSnake(oldX, oldY);
        gameField[oldY][oldX] = 'B';
        this->game->decreaseFood();
    }
    else
        gameField[oldY][oldX] = FLOOR_SYMBOL;

    if (gameField[head->y][head->x] == 'B' || gameField[head->y][head->x] == 'H')
        isDead = true;

    if (isDead)
        return this->game->addDeadSnake(this->fd);

    gameField[head->y][head->x] = 'H';
    this->headX.store(head->x);
    this->headY.store(head->y);
}

void Snake::setDirection(const int newDir)
{
    enum e_direction dir = (enum e_direction)newDir;
    if ((dir == UP || dir == DOWN) && (this->direction == DOWN || this->direction == UP))
        return;
    if ((dir == RIGHT || dir == LEFT) && (this->direction == RIGHT || this->direction == LEFT))
        return;

    this->direction = dir;
}

void Snake::growSnake(const int oldX, const int oldY)
{
    struct snake *newTail = (struct snake *)malloc(sizeof(struct snake));
    newTail->next = NULL;
    newTail->x = oldX;
    newTail->y = oldY;
    newTail->prev = this->tail;
    this->tail->next = newTail;

    this->tail = newTail;
}

void Snake::cleanSnakeFromField(std::vector<std::string> &gameField)
{
    struct snake *currentPart = this->tail;
    while (currentPart)
    {
        gameField[currentPart->y][currentPart->x] = FLOOR_SYMBOL;
        currentPart = currentPart->prev;
    }
}

void Snake::updateGameSize(const int height, const int width)
{
    this->gameHeight = height;
    this->gameWidth = width;
}

in_addr_t Snake::getClientAddress() const
{
    return this->clientAddr;
}

int Snake::getHeadX() const
{
    return this->headX.load();
}

int Snake::getHeadY() const
{
    return this->headY.load();
}

int Snake::getFd() const
{
    return this->fd;
}
