#include "Drawer.hpp"

#define WIDTH 1000
#define HEIGHT 1000
#define INITIAL_SCREEN_SIZE 20
#define SCREEN_LEN 2.0f

Drawer::Drawer(Client *client) : client(client), screenSize(INITIAL_SCREEN_SIZE),
                                 prevSnakeHeadX(0), prevSnakeHeadY(0), switchLibPath("../libs/lib2/lib2"), gameMode(MENU)
{
    tilePx = std::max(1, std::min(WIDTH / screenSize, HEIGHT / screenSize));

    assets = (char **)malloc(sizeof(char *) * 3);
    assets[0] = (char *)malloc(17);
    assets[0] = strdup("assets/body.png");
    assets[1] = (char *)malloc(17);
    assets[1] = strdup("assets/head.png");
    assets[2] = (char *)malloc(17);
    assets[2] = strdup("assets/food.png");
    assets[3] = NULL;
}

Drawer::~Drawer()
{
    this->closeDynamicLib();

    for (int i = 0; assets[i]; ++i)
        free(assets[i]);
    free(assets);
}

void Drawer::loadDynamicLibrary(const std::string &lib)
{
    std::string libPath = lib + LIB_EXTENSION;
    std::cout << libPath << std::endl;
    this->dynamicLibrary = dlopen(libPath.c_str(), RTLD_LAZY);
    if (!this->dynamicLibrary)
        throw "Failed to load dynlib";

    dlerror(); // clean errors

    this->init = (initFunc)dlsym(this->dynamicLibrary, "init");
    this->cleanup = (cleanupFunc)dlsym(this->dynamicLibrary, "cleanup");
    this->loadAssets = (loadAssetsFunc)dlsym(this->dynamicLibrary, "loadAssets");
    this->drawAsset = (drawAssetFunc)dlsym(this->dynamicLibrary, "drawAsset");
    this->drawButton = (drawButtonFunc)dlsym(this->dynamicLibrary, "drawButton");
    this->drawText = (drawTextFunc)dlsym(this->dynamicLibrary, "drawText");
    this->beginFrame = (beginFrameFunc)dlsym(this->dynamicLibrary, "beginFrame");
    this->endFrame = (endFrameFunc)dlsym(this->dynamicLibrary, "endFrame");
    this->checkEvents = (checkEventsFunc)dlsym(this->dynamicLibrary, "checkEvents");

    char *error = dlerror(); // check dlsym calls
    if (error != NULL)
        throw "Failed to find functions in dynlib";

    if (!this->init || !this->cleanup || !this->drawAsset ||
        !this->drawButton || !this->drawText || !this->loadAssets ||
        !this->endFrame || !this->beginFrame || !this->checkEvents)
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
            this->loadAssets(this->window, (const char **)assets);
            gameRunning = true;

            while (gameRunning)
            {
                this->beginFrame(this->window);

                t_event event = this->checkEvents(this->window);
                switch (event.type)
                {
                case EXIT:
                    gameRunning = false;
                    break;
                case KEY:
                    onKeyPress(event.a);
                    break;
                case MOUSE:
                    onMouseUp(event.a, event.b);
                    break;
                default:
                    break;
                }

                if (this->gameMode == GAME)
                    this->drawGameField();
                else
                    this->drawMenu();

                this->endFrame(this->window);
            }

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

void Drawer::openWindow()
{
    this->window = this->init(HEIGHT, WIDTH, this);
    if (!this->window)
        throw("Failed to init lib");
}

void Drawer::drawMenu()
{
    this->drawText(this->window, 380, 200, 40, "42 SNAKES");
    this->drawButton(this->window, 400, 300, 200, 60, "Multiplayer");
    this->drawButton(this->window, 400, 400, 200, 60, "Single-player");
}

void Drawer::drawGameField()
{
    if (this->client->getIsDead() || this->client->getStopFlag())
    {
        this->stopClient();
        this->gameMode = MENU;
        return;
    }

    const int snakeHeadX = this->client->getSnakeX();
    const int snakeHeadY = this->client->getSnakeY();

    std::mutex &gameFieldMutex = this->client->getGameFieldMutex();
    std::lock_guard<std::mutex> lock(gameFieldMutex);

    const std::vector<std::string> &gameField = this->client->getGameField();
    this->height = this->client->getHeight();
    this->width = this->client->getWidth();
    const int screenCenter = screenSize / 2;
    const int originX = (WIDTH - (tilePx * screenSize)) / 2;
    const int originY = (HEIGHT - (tilePx * screenSize)) / 2;

    for (int sy = 0; sy < screenSize; ++sy)
    {
        int wy = snakeHeadY + (sy - screenCenter);
        for (int sx = 0; sx < screenSize; ++sx)
        {
            int wx = snakeHeadX + (sx - screenCenter);
            if (wx < 0 || wx >= width || wy < 0 || wy >= height)
                continue;

            int px = originX + sx * tilePx;
            int py = originY + sy * tilePx;

            this->drawBorder(wx, wy, px, py, tilePx);

            char tile = gameField[wy][wx];
            if (tile == 'F')
                this->drawAsset(this->window, px, py, tilePx, tilePx, "assets/food.png");
            else if (tile == 'B')
                this->drawAsset(this->window, px, py, tilePx, tilePx, "assets/body.png");
            else if (tile == 'H')
                this->drawAsset(this->window, px, py, tilePx, tilePx, "assets/head.png");
            else
                continue;
        }
    }
    this->prevSnakeHeadX = snakeHeadX;
    this->prevSnakeHeadY = snakeHeadY;
}

void Drawer::drawBorder(int x, int y, int px, int py, int tilePx)
{
    if (x == 0)
        this->drawAsset(this->window, px - tilePx, py, tilePx, tilePx, "assets/body.png");

    if (x == this->width - 1)
        this->drawAsset(this->window, px + tilePx, py, tilePx, tilePx, "assets/body.png");

    if (y == 0)
        this->drawAsset(this->window, px, py - tilePx, tilePx, tilePx, "assets/body.png");

    if (y == this->height - 1)
        this->drawAsset(this->window, px, py + tilePx, tilePx, tilePx, "assets/body.png");
}

void Drawer::stopClient()
{
    this->client->setStopFlag(true);
    if (this->clientThread.joinable())
        this->clientThread.join();
}

void Drawer::onMouseUp(float x, float y)
{
    (void)x;
    (void)y;
    if (!clientThread.joinable())
    {
        this->client->setIsDead(false);
        this->client->setStopFlag(false);
        std::cout << "Starting client" << std::endl;
        this->clientThread = std::thread(&Client::start, this->client);
        this->gameMode = GAME;
    }
}

void Drawer::onKeyPress(int key)
{
    actions action = (actions)key;

    switch (action)
    {
    case UP:
    case DOWN:
    case RIGHT:
    case LEFT:
        this->client->sendDirection(action);
        break;
    case M:
        this->screenSize = this->screenSize * 1.10 + 0.5;
        this->tilePx = std::max(1, std::min(WIDTH / screenSize, HEIGHT / screenSize));
        break;
    case N:
        this->screenSize = this->screenSize / 1.10;
        this->tilePx = std::max(1, std::min(WIDTH / screenSize, HEIGHT / screenSize));
        break;
    case KEY_1:
        this->switchLibPath = "../libs/lib1/lib1";
        this->gameRunning = false;
        break;
    case KEY_2:
        this->switchLibPath = "../libs/lib2/lib2";
        this->gameRunning = false;
        break;
    case KEY_3:
        this->switchLibPath = "../libs/lib4/lib3";
        this->gameRunning = false;
        break;
    }
}
