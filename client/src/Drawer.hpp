#ifndef DRAWER_HPP
#define DRAWER_HPP

#include "../includes/nibbler.hpp"
#include "Client.hpp"

class Client;

struct s_position
{
    int x;
    int y;
};

class Drawer
{
public:
    Drawer(Client *client);
    ~Drawer();

    void start();

    void loadDynamicLibrary(const char *lib);
    void keyCallback(int key, int action);
    void drawGameField();

private:
    Client *client;
    void *dynamicLibrary;
    void *window;
    int screenSize;
    int height;
    int width;

    initFunc init;
    loopFunc loop;
    drawSquareFunc drawSquare;
    cleanupFunc cleanup;

    void openWindow();
    void printField() const;
    void drawBorder(int x, int y, float windowX, float windowY, float step);
    struct s_position findSnakeHead(const std::vector<std::string> &gameField) const;
};

#endif
