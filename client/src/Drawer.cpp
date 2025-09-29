#include "Drawer.hpp"

#define WIDTH 1000
#define HEIGHT 1000
#define SCREEN_LEN 2.0f

Drawer::Drawer(Client *client) : client(client), screenSize(20), prevSnakeHeadX(0), isMenuDrawn(false),
                                 prevSnakeHeadY(0), window(nullptr), gameMode(MENU), dynamicLibrary(nullptr),
                                 switchLibPath("/Users/tolmadan/Desktop/42/nibbler/libs/lib2/lib2")
{
}

Drawer::~Drawer()
{
    this->closeDynamicLib();
}

void Drawer::loadDynamicLibrary(const std::string &lib)
{
    std::string libPath = lib + LIB_EXTENSION;
    this->dynamicLibrary = dlopen(libPath.c_str(), RTLD_LAZY);
    if (!this->dynamicLibrary)
        throw "Failed to load dynlib";

    dlerror(); // clean errors

    this->init = (initFunc)dlsym(this->dynamicLibrary, "init");
    this->loop = (loopFunc)dlsym(this->dynamicLibrary, "loop");
    this->cleanup = (cleanupFunc)dlsym(this->dynamicLibrary, "cleanup");
    this->closeWindow = (closeWindowFunc)dlsym(this->dynamicLibrary, "closeWindow");
    this->drawSquare = (drawSquareFunc)dlsym(this->dynamicLibrary, "drawSquare");
    this->drawButton = (drawButtonFunc)dlsym(this->dynamicLibrary, "drawButton");
    this->drawText = (drawTextFunc)dlsym(this->dynamicLibrary, "drawText");
    this->display = (displayFunc)dlsym(this->dynamicLibrary, "display");
    this->cleanScreen = (cleanScreenFunc)dlsym(this->dynamicLibrary, "cleanScreen");

    char *error = dlerror(); // check dlsym calls
    if (error != NULL)
        throw "Failed to find functions in dynlib";

    // TODO: add draw button & text
    if (!this->init || !this->loop || !this->cleanup || !this->drawSquare || !this->closeWindow || !this->display || !this->cleanScreen)
        throw "Failed to init dynlib functions";
}

void Drawer::closeDynamicLib()
{
    if (this->window)
    {
        this->cleanup(this->window);
        this->window = nullptr;
    }
    if (this->dynamicLibrary)
    {
        dlclose(this->dynamicLibrary);
        this->dynamicLibrary = nullptr;
    }
}

void Drawer::startDynamicLib()
{
    this->closeDynamicLib();
    this->loadDynamicLibrary(this->switchLibPath);
    this->switchLibPath = "";
}

void Drawer::start()
{
    try
    {
        while (1)
        {
            this->startDynamicLib();
            this->openWindow();
            this->loop(this->window);

            if (this->switchLibPath.empty())
                break;
        }
    }
    catch (const char *msg)
    {
        std::cerr << msg << std::endl;
    }

    this->stopClient();
}

void Drawer::onEachFrame()
{
    if (this->gameMode == MENU)
        this->drawMenu();
    else
        this->drawGameField();
}

void Drawer::openWindow()
{
    this->window = this->init(HEIGHT, WIDTH, this);
    if (!this->window)
        throw("Failed to init lib");
}

void Drawer::drawMenu()
{
    if (this->isMenuDrawn)
        return;

    this->cleanScreen(this->window);
    this->drawText(this->window, 380, 200, 40, "42 SNAKES");
    this->drawButton(this->window, 400, 300, 200, 60, "Multiplayer");
    this->drawButton(this->window, 400, 400, 200, 60, "Single-player");
    this->display(this->window);

    this->isMenuDrawn = true;
}

void Drawer::drawGameField()
{
    if (this->client->getIsDead())
    {
        this->stopClient();
        this->gameMode = MENU;
        return;
    }

    if (this->client->getStopFlag())
        throw "Client has stopped";

    const int snakeHeadX = this->client->getSnakeX();
    const int snakeHeadY = this->client->getSnakeY();
    if (snakeHeadX == this->prevSnakeHeadX && snakeHeadY == this->prevSnakeHeadY)
        return;

    std::mutex &gameFieldMutex = this->client->getGameFieldMutex();
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
                this->rgb = {0.5f, 0.1f, 0.1f};
            else if (tile == 'B')
                this->rgb = {0.0f, 0.6f, 0.2f};
            else if (tile == 'H')
                this->rgb = {0.9f, 0.3f, 0.0f};
            else
                continue;

            this->drawSquare(this->window, windowX, windowY, step, step, this->rgb);
        }
    }

    this->prevSnakeHeadX = snakeHeadX;
    this->prevSnakeHeadY = snakeHeadY;

    this->display(this->window);
}

void Drawer::drawBorder(int x, int y, float windowX, float windowY, float step)
{
    this->rgb = {7.0f, 7.0f, 7.0f};

    if (x == 0)
        this->drawSquare(this->window, windowX - step, windowY, step, step, this->rgb);
    if (x == this->width - 1)
        this->drawSquare(this->window, windowX + step, windowY, step, step, this->rgb);
    if (y == 0)
        this->drawSquare(this->window, windowX, windowY + step, step, step, this->rgb);
    if (y == this->height - 1)
        this->drawSquare(this->window, windowX, windowY - step, step, step, this->rgb);
}

void Drawer::stopClient()
{
    this->client->stop();
    if (this->clientThread.joinable())
        this->clientThread.join();
}

void Drawer::onMouseUp(float x, float y)
{
    if (!clientThread.joinable())
    {
        std::cout << "Starting client" << std::endl;
        this->clientThread = std::thread(&Client::start, this->client);
        this->gameMode = GAME;
        this->isMenuDrawn = false;
    }
}

void Drawer::keyCallback(actions key, int action)
{
    if (action == 1)
    {
        switch (key)
        {
        case UP:
        case DOWN:
        case RIGHT:
        case LEFT:
            this->client->sendDirection(key);
            break;
        case M:
            this->screenSize = this->screenSize * 1.10 + 0.5;
            break;
        case N:
            this->screenSize = this->screenSize / 1.10;
            break;
        case KEY_1:
            std::cout << "Changing to LIB 1" << std::endl;
            this->switchLibPath = "/Users/tolmadan/Desktop/42/nibbler/libs/lib1/lib1";
            this->closeWindow(this->window);
            break;
        case KEY_2:
            std::cout << "Changing to LIB 2" << std::endl;
            this->switchLibPath = "/Users/tolmadan/Desktop/42/nibbler/libs/lib2/lib2";
            this->closeWindow(this->window);
            break;
        }
    }
}
