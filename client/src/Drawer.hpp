#ifndef DRAWER_HPP
#define DRAWER_HPP

#include "../includes/nibbler.hpp"
#include "Client.hpp"

enum mode
{
    MENU,
    GAME
};

class Client;

class Drawer
{
public:
    Drawer(Client *client);
    ~Drawer();

    void start();

    void loadDynamicLibrary(const std::string &lib);
    void keyCallback(actions key, int action);
    void drawGameField();
    void drawMenu();
    void onEachFrame();
    void onMouseUp(float x, float y);

private:
    Client *client;
    void *dynamicLibrary;
    void *window;
    int screenSize;
    int tilePx;
    int height;
    int width;
    int prevSnakeHeadX;
    int prevSnakeHeadY;
    bool isMenuDrawn;
    std::string switchLibPath;
    std::thread clientThread;
    mode gameMode;
    struct rgb rgb;

    initFunc init;
    loopFunc loop;
    displayFunc display;
    cleanScreenFunc cleanScreen;
    closeWindowFunc closeWindow;
    drawSquareFunc drawSquare;
    drawButtonFunc drawButton;
    drawTextFunc drawText;
    cleanupFunc cleanup;

    void stopClient();
    void openWindow();
    void startDynamicLib();
    void closeDynamicLib();
    void drawBorder(int x, int y, int px, int py, int tilePx);
};

#endif
