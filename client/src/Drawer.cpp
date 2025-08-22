#include "Drawer.hpp"

#define WIDTH 1000
#define HEIGHT 1000
#define SCREEN_LEN 2
#define GAME_SIZE_X 20
#define GAME_SIZE_y 20

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
    this->window = this->init(1000, 1000, this);
    if (!this->window)
        onerror("Failed to init lib");
}

void Drawer::drawGameField()
{
    struct rgb rgb;

    std::mutex &gameFieldMutex = this->client->getGameFieldMutex();
    std::lock_guard<std::mutex> lock(gameFieldMutex);

    const std::vector<std::string> &gameField = this->client->getGameField();
    const int height = this->client->getHeight();
    const int width = this->client->getWidth();
    const float step = (float)SCREEN_LEN / height;
    std::cout << step << std::endl;

    for (int y = 0; y < height; y++)
    {
        float windowY = 1.0f - (float)y * step; // TODO: 1.0f - smth
        for (int x = 0; x < width; x++)
        {
            float windowX = -1.0f + (float)x * step;
            char tile = gameField[y][x];
            if (tile == 'F')
                rgb = {0.5f, 0.1f, 0.1f};
            else if (tile == 'B')
                rgb = {0.0f, 0.6f, 0.2f};
            else if (tile == 'H')
                rgb = {0.9f, 0.3f, 0.0f};
            else
                continue;

            this->drawSquare(this->window, windowX, windowY, step, rgb);
        }
    }
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
