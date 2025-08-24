#include "Drawer.hpp"

#define WIDTH 1000
#define HEIGHT 1000
#define SCREEN_LEN 2
#define SCREEN_SIZE 20
#define STEP 0.1f

Drawer::Drawer(Client *client)
{
    this->client = client;
}

Drawer::~Drawer()
{
    this->cleanup(this->window);
    dlclose(this->dynamicLibrary);
}

void Drawer::loadDynamicLibrary(const char *lib)
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

void Drawer::start()
{
    this->openWindow();
    this->loop(this->window);
}

void Drawer::openWindow()
{
    this->window = this->init(HEIGHT, WIDTH, this);
    if (!this->window)
        onerror("Failed to init lib");
}

void Drawer::drawGameField()
{
    struct rgb rgb;

    std::mutex &gameFieldMutex = this->client->getGameFieldMutex();
    std::lock_guard<std::mutex> lock(gameFieldMutex);

    const std::vector<std::string> &gameField = this->client->getGameField();
    struct s_position pos = this->findSnakeHead(gameField);
    const int height = this->client->getHeight();
    const int width = this->client->getWidth();

    const int centerX = SCREEN_SIZE / 2;
    const int centerY = SCREEN_SIZE / 2;

    for (int y = 0; y < SCREEN_SIZE; y++)
    {
        int offsetY = y - pos.y;
        int drawY = pos.y + offsetY;
        float windowY = 1.0f - (centerY + offsetY) * STEP;

        for (int x = 0; x < SCREEN_SIZE; x++)
        {
            int offsetX = x - pos.x;
            float windowX = -1.0f + (centerX + offsetX) * STEP;
            int drawX = pos.x + offsetX;

            if (drawX < 0 || drawX > width || drawY < 0 || drawY > height)
                continue;

            this->drawBorder(drawX, drawY, windowX, windowY);

            char tile = gameField[drawY][drawX];

            if (tile == 'F')
                rgb = {0.5f, 0.1f, 0.1f};
            else if (tile == 'B')
                rgb = {0.0f, 0.6f, 0.2f};
            else if (tile == 'H')
                rgb = {0.9f, 0.3f, 0.0f};
            else
                continue;

            this->drawSquare(this->window, windowX, windowY, STEP, STEP, rgb);
        }
    }
}

void Drawer::drawBorder(int x, int y, float windowX, float windowY)
{
    rgb rgb = {1.0f, 0.0f, 0.0f};
    float t = STEP; // thinner, change as needed

    // world border
    if (x == 0)
    {
        this->drawSquare(this->window, windowX - (STEP), windowY, t, STEP, rgb);
    }
    if (x == SCREEN_SIZE - 1)
    {
        this->drawSquare(this->window, windowX + (STEP), windowY, t, STEP, rgb);
    }
    if (y == 0)
    {
        this->drawSquare(this->window, windowX, windowY - (STEP * 0.5f) + (t * 0.5f), STEP, t, rgb);
    }
    if (y == SCREEN_SIZE - 1)
    {
        this->drawSquare(this->window, windowX, windowY + (STEP * 0.5f) - (t * 0.5f), STEP, t, rgb);
    }
}

struct s_position Drawer::findSnakeHead(const std::vector<std::string> &gameField) const
{
    struct s_position pos;
    pos.x = -1;
    pos.y = -1;

    for (auto iter = gameField.begin(); iter != gameField.end(); iter++)
    {
        ssize_t foundPos = iter->find_first_of('H');
        if (foundPos != std::string::npos)
        {
            pos.x = foundPos;
            pos.y = std::distance(gameField.begin(), iter);
            break;
        }
    }

    return pos;
}

void Drawer::keyCallback(int key, int action)
{
    if (action == 1)
    {
        switch (key)
        {
        case 265:
            this->client->setDirection(UP);
            break;
        case 264:
            this->client->setDirection(DOWN);
            break;
        case 263:
            this->client->setDirection(LEFT);
            break;
        case 262:
            this->client->setDirection(RIGHT);
            break;
        }
    }
}
