#ifndef DRAWER_HPP
#define DRAWER_HPP

#include "../includes/nibbler.hpp"
#include "Client.hpp"

class Client;

class Drawer
{
public:
    Drawer(const Client *client);
    ~Drawer();

    void loadDynamicLibrary(const char *lib);
    void keyCallback(int key, int action);
    void start();
    void drawGameField();

private:
    const Client *client;
    void *dynamicLibrary;
    void *window;
    enum direction direction;
    initFunc init;
    loopFunc loop;
    drawSquareFunc drawSquare;
    cleanupFunc cleanup;

    void openWindow();
    void printField() const;
};

#endif
