#ifndef DRAWER_HPP
#define DRAWER_HPP

#include "../includes/nibbler.hpp"
#include "Client.hpp"

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

private:
    Client *client;
    void *dynamicLibrary;
    void *window;
    int screenSize;
    int height;
    int width;
    int prevSnakeHeadX;
    int prevSnakeHeadY;
    std::string switchLibPath;
    struct rgb rgb;

    initFunc init;
    loopFunc loop;
    displayFunc display;
    cleanScreenFunc cleanScreen;
    closeWindowFunc closeWindow;
    drawSquareFunc drawSquare;
    cleanupFunc cleanup;

    void openWindow();
    void switchDynamicLib();
    void closeDynamicLib();
    void drawBorder(int x, int y, float windowX, float windowY, float step);
};

#endif
