#ifndef NIBBLER_HPP
#define NIBBLER_HPP

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <dlfcn.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <poll.h>
#include <fcntl.h>
#include <vector>
#include <iostream>
#include <thread>

#define TILE_SIZE 0.05f
#define SCALE 20.0f
#define FLOOR_SYMBOL '.'

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
