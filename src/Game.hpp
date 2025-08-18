#ifndef GAME_HPP
#define GAME_HPP

#include "../includes/nibbler.hpp"
#include <vector>
#include "Snake.hpp"

class Snake;

class Game
{
public:
    Game(int height, int width);
    ~Game();

    void addSnake(Snake *snake);
    void loadDynamicLibrary(const char *lib);
    void openWindow();
    void startGameLoop();
    void printField() const;
    void draw(float x, float y, float size, struct rgb rgb) const;
    void keyCallback(int key, int action);
    void gameLoop();
    void decreaseFood();

    char **field;

private:
    Snake *snake;
    int width;
    int height;
    int foodCount = 0;
    void *window;
    void *dynamicLibrary;

    drawSquareFunc drawSquare;
    cleanupFunc cleanup;
    loopFunc loop;
    initFunc init;

    void spawnFood();
    void drawFood();
};

#endif
