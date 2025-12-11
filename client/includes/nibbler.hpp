#ifndef NIBBLER_HPP
#define NIBBLER_HPP

#include <arpa/inet.h>
#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <dlfcn.h>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <signal.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <vector>

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

enum class StateType { Global, Menu, Game, GameOver, Paused };

enum actions { UP, DOWN, LEFT, RIGHT, M, N, KEY_1, KEY_2, KEY_3 };

struct Vec2i {
  int x;
  int y;
};

enum type {
  CLOSED,
  RESIZED,
  FOCUS_LOST,
  FOCUS_GAINED,
  TEXT_ENTERED,
  KEY_PRESSED,
  KEY_RELEASED,
  MOUSE_WHEEL_SCROLLED,
  MOUSE_BUTTON_PRESSED,
  MOUSE_BUTTON_RELEASED,
  MOUSE_MOVED,
  MOUSE_MOVED_RAW,
  MOUSE_ENTERED,
  MOUSE_LEFT,
  EMPTY = 99
};

typedef struct s_event {
  type type;
  union {
    int keyCode;
    struct {
      int x, y;
      int button;
    } mouse;
    struct {
      int width, height;
    } window;
    int wheelDelta;
    std::uint32_t unicode;
  };
} t_event;

typedef void* (*initFunc)(int height, int width, void* userData);
typedef void (*loopFunc)(void* window);
typedef void (*beginFrameFunc)(void* window);
typedef void (*endFrameFunc)(void* window);
typedef void (*cleanupFunc)(void* window);
typedef t_event (*checkEventsFunc)(void* window);
typedef void (*loadAssetsFunc)(void* window, const char** paths);
typedef void (*drawTextFunc)(void* window, float x, float y, int size, const char* text);
typedef void (*drawAssetFunc)(void* window, float x, float y, float width, float height,
                              int degrees, const char* assetPath);
typedef void (*drawButtonFunc)(void* window, float x, float y, float width, float height,
                               const char* text);

#endif
