#ifndef NIBBLER_HPP
#define NIBBLER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <poll.h>
#include <iostream>
#include <vector>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include <dlfcn.h>
#include <thread>
#include <atomic>
#include <mutex>

#define TILE_SIZE 0.05f
#define SCALE 20.0f
#define FLOOR_SYMBOL '.'

#ifdef __APPLE__
#define LIB_EXTENSION ".dylib"
#elif __linux__
#define LIB_EXTENSION ".so"
#endif

enum actions
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    M,
    N,
    KEY_1,
    KEY_2,
    KEY_3
};

struct rgb
{
    float r;
    float g;
    float b;
};

typedef void *(*initFunc)(int, int, void *);
typedef void (*loopFunc)(void *);
typedef void (*displayFunc)(void *);
typedef void (*cleanScreenFunc)(void *);
typedef void (*cleanupFunc)(void *);
typedef void (*stopLibraryFunc)(void *);
typedef void (*drawTextFunc)(void *, float, float, int, const char *);
typedef void (*drawSquareFunc)(void *, float, float, float, float, struct rgb);
typedef void (*drawButtonFunc)(void *, float, float, float, float, const char *);

#endif
