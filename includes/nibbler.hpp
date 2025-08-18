#ifndef NIBBLER_HPP
#define NIBBLER_HPP

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <dlfcn.h>
#include <time.h>

#define TILE_SIZE 0.05f
#define SCALE 20.0f

class Game;

struct rgb
{
    float r;
    float g;
    float b;
};

typedef void *(*initFunc)(int, int, void *);
typedef void (*loopFunc)(void *);
typedef void (*cleanupFunc)(void *);
typedef void (*drawSquareFunc)(void *, float, float, float, struct rgb);

#include "../src/Game.hpp"

// utils
[[noreturn]] void onerror(const char *msg);

#endif
