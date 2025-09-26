#include "Drawer.hpp"

#define WIDTH 1000
#define HEIGHT 1000
#define SCREEN_LEN 2.0f

Drawer::Drawer(Client *client)
{
    this->client = client;
    this->screenSize = 20;
    this->prevSnakeHeadX = 0;
    this->prevSnakeHeadY = 0;
}

Drawer::~Drawer()
{
    this->cleanup(this->window);
    dlclose(this->dynamicLibrary);
}

void Drawer::loadDynamicLibrary(const std::string &lib)
{
    std::string libPath = lib + LIB_EXTENSION;
    this->dynamicLibrary = dlopen(libPath.c_str(), RTLD_LAZY);
    if (!this->dynamicLibrary)
        onerror("Failed to load dynlib");

    dlerror(); // clean errors

    this->init = (initFunc)dlsym(this->dynamicLibrary, "init");
    this->loop = (loopFunc)dlsym(this->dynamicLibrary, "loop");
    this->cleanup = (cleanupFunc)dlsym(this->dynamicLibrary, "cleanup");
    this->closeWindow = (closeWindowFunc)dlsym(this->dynamicLibrary, "closeWindow");
    this->drawSquare = (drawSquareFunc)dlsym(this->dynamicLibrary, "drawSquare");
    this->display = (displayFunc)dlsym(this->dynamicLibrary, "display");
    this->cleanScreen = (cleanScreenFunc)dlsym(this->dynamicLibrary, "cleanScreen");

    char *error = dlerror(); // check dlsym calls
    if (error != NULL)
        onerror("Failed to find functions in dynamic lib");

    if (!this->init || !this->loop || !this->cleanup || !this->drawSquare || !this->closeWindow || !this->display || !this->cleanScreen)
        onerror("Failed to init lib functions");
}

void Drawer::start()
{
    this->openWindow();
    this->loop(this->window);
    std::cout << "Loop ended" << std::endl;
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
    const int snakeHeadX = this->client->getSnakeX();
    const int snakeHeadY = this->client->getSnakeY();
    if (snakeHeadX == this->prevSnakeHeadX && snakeHeadY == this->prevSnakeHeadY)
        return;

    std::lock_guard<std::mutex> lock(gameFieldMutex);
    const std::vector<std::string> &gameField = this->client->getGameField();
    this->height = this->client->getHeight();
    this->width = this->client->getWidth();
    const int screenCenter = screenSize / 2;
    const float step = SCREEN_LEN / screenSize;

    this->cleanScreen(this->window);

    for (int sy = 0; sy < this->screenSize; sy++)
    {
        int wy = snakeHeadY + (sy - screenCenter);
        float windowY = 1.0f - (sy + 0.5f) * step;
        for (int sx = 0; sx < this->screenSize; sx++)
        {
            int wx = snakeHeadX + (sx - screenCenter);
            float windowX = -1.0f + (sx + 0.5f) * step;

            if (wx < 0 || wx >= width || wy < 0 || wy >= height)
                continue;

            this->drawBorder(wx, wy, windowX, windowY, step);

            char tile = gameField[wy][wx];
            if (tile == 'F')
                rgb = {0.5f, 0.1f, 0.1f};
            else if (tile == 'B')
                rgb = {0.0f, 0.6f, 0.2f};
            else if (tile == 'H')
                rgb = {0.9f, 0.3f, 0.0f};
            else
                continue;

            this->drawSquare(this->window, windowX, windowY, step, step, rgb);
        }
    }

    this->prevSnakeHeadX = snakeHeadX;
    this->prevSnakeHeadY = snakeHeadY;

    this->display(this->window);
}

void Drawer::drawBorder(int x, int y, float windowX, float windowY, float step)
{
    rgb rgb = {7.0f, 7.0f, 7.0f};

    if (x == 0)
        this->drawSquare(this->window, windowX - step, windowY, step, step, rgb);
    if (x == this->width - 1)
        this->drawSquare(this->window, windowX + step, windowY, step, step, rgb);
    if (y == 0)
        this->drawSquare(this->window, windowX, windowY + step, step, step, rgb);
    if (y == this->height - 1)
        this->drawSquare(this->window, windowX, windowY - step, step, step, rgb);
}

void Drawer::keyCallback(int key, int action)
{
    if (action == 1)
    {
        switch (key)
        {
        case 265:
            this->client->sendDirection(UP);
            break;
        case 264:
            this->client->sendDirection(DOWN);
            break;
        case 263:
            this->client->sendDirection(LEFT);
            break;
        case 262:
            this->client->sendDirection(RIGHT);
            break;
        case 77: // M
            // std::cout << "Changing lib!" << std::endl;
            // this->closeWindow(this->window);
            // this->cleanup(this->window);
            // dlclose(this->dynamicLibrary);
            // this->loadDynamicLibrary("/Users/tolmadan/Desktop/42/nibbler/libs/lib2/lib2");
            std::cout << "Starting lib!" << std::endl;

            // this->start();
            this->screenSize = this->screenSize * 1.10 + 0.5;
            break;
        case 78: // N
            this->screenSize = this->screenSize / 1.10;
        }
    }
}
