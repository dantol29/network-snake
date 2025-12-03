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
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <cstdlib>

#define TILE_SIZE 0.05f
#define SCALE 20.0f
#define FLOOR_SYMBOL '.'
#define LOCAL_SERVER_IP "127.0.0.1"
#define REMOTE_SERVER_IP "159.65.186.248"
#define DEFAULT_GAME_HEIGHT 20
#define DEFAULT_GAME_WIDTH 30

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

enum type {
    KEY,
    MOUSE,
    EXIT,
    EMPTY
};

typedef struct s_event {
    type type;
    int a;
    int b;
} t_event;

typedef void *(*initFunc)(int, int, void *);
typedef void (*loopFunc)(void *);
typedef void (*beginFrameFunc)(void *);
typedef void (*endFrameFunc)(void *);
typedef void (*cleanupFunc)(void *);
typedef t_event (*checkEventsFunc)(void *);
typedef void (*loadAssetsFunc)(void *, const char **);
typedef void (*drawTextFunc)(void *, float, float, int, const char *);
typedef void (*drawAssetFunc)(void *, float, float, float, float, const char *);
typedef void (*drawButtonFunc)(void *, float, float, float, float, const char *);

#endif
