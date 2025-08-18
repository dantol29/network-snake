#include "Game.hpp"
#include <chrono>

#define MAX_FOOD_SPAWN_TRIES 50

using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;

TimePoint lastMoveTime = Clock::now();
TimePoint lastEatTime = Clock::now();

Game::Game(int width, int height) : width(width), height(height)
{
    this->field = (char **)malloc(sizeof(char *) * this->height + 1);
    if (!this->field)
        onerror("Memory error");

    for (int i = 0; i < this->height; i++)
    {
        this->field[i] = (char *)malloc(sizeof(char) * this->width + 1);
        if (!this->field[i])
            onerror("Memory error");

        for (int j = 0; j < this->width; j++)
            this->field[i][j] = '0';
        this->field[i][this->width] = '\0';
    }
    this->field[this->height] = NULL;

    srand(time(NULL)); // init random generator
}

Game::~Game()
{
    this->cleanup(this->window);
    dlclose(this->dynamicLibrary);

    for (int i = 0; this->field[i]; i++)
        free(this->field[i]);
    free(this->field);
}

void Game::loadDynamicLibrary(const char *lib)
{
    this->dynamicLibrary = dlopen(lib, RTLD_LAZY);
    if (!this->dynamicLibrary)
        onerror("Failed to load dynlib");

    dlerror(); // clean errors

    this->init = (initFunc)dlsym(this->dynamicLibrary, "init");
    this->loop = (loopFunc)dlsym(this->dynamicLibrary, "loop");
    this->cleanup = (cleanupFunc)dlsym(this->dynamicLibrary, "cleanup");
    this->drawSquare = (drawSquareFunc)dlsym(this->dynamicLibrary, "drawSquare");

    char *error = dlerror(); // check dlsym calls
    if (error != NULL)
        onerror("Failed to find functions in dynamic lib");

    if (!this->init || !this->loop || !this->cleanup || !this->drawSquare)
        onerror("Failed to init lib functions");
}

void Game::keyCallback(int key, int action)
{
    if (action == 1)
    {
        switch (key)
        {
        case 265:
            this->snake->setDirection(UP);
            break;
        case 264:
            this->snake->setDirection(DOWN);
            break;
        case 263:
            this->snake->setDirection(LEFT);
            break;
        case 262:
            this->snake->setDirection(RIGHT);
            break;
        }
    }
}

void Game::drawFood()
{
    struct rgb rgb;

    for (int y = 0; this->field[y]; y++)
    {
        for (int x = 0; this->field[y][x]; x++)
        {
            if (this->field[y][x] == 'F')
            {
                float windowX = -1.0f + (float)x / SCALE;
                float windowY = 0.9f - (float)y / SCALE;
                rgb = {0.5f, 0.1f, 0.1f};

                this->draw(windowX, windowY, TILE_SIZE, rgb);
            }
        }
    }
}

void Game::spawnFood()
{
    if (foodCount > 1)
        return;

    for (int i = 0; i < MAX_FOOD_SPAWN_TRIES; i++)
    {
        int r1 = rand();
        int r2 = rand();
        int x = r1 % this->width;
        int y = r2 % this->height;

        if (this->field[y][x] == '0')
        {
            this->field[y][x] = 'F';
            ++foodCount;
            return;
        }
    }
}

void Game::gameLoop()
{
    auto now = Clock::now();
    bool canMove = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMoveTime).count() >= 300;
    bool canEat = std::chrono::duration_cast<std::chrono::seconds>(now - lastEatTime).count() >= 3;

    if (canMove)
    {
        this->snake->moveSnake();
        this->printField();
        lastMoveTime = now;
    }

    if (canEat)
    {
        this->spawnFood();
        lastEatTime = now;
    }

    this->drawFood();
    this->snake->drawSnake();
}

void Game::openWindow()
{
    this->window = this->init(800, 600, this);
    if (!this->window)
        onerror("Failed to init lib");
}

void Game::startGameLoop()
{
    this->loop(this->window);
}

void Game::draw(float x, float y, float size, struct rgb rgb) const
{
    this->drawSquare(this->window, x, y, size, rgb);
}

void Game::addSnake(Snake *snake)
{
    this->snake = snake;
}

void Game::decreaseFood()
{
    if (this->foodCount > 0)
        this->foodCount--;
}

void Game::printField() const
{
    if (!this->field)
        return;

    printf("\n\n");
    for (int i = 0; this->field[i]; i++)
        printf("%3d:%s\n", i, this->field[i]);
    printf("\n\n");
}
