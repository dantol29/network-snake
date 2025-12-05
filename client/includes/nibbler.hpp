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
#include <cstdint>

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

// Created to accommodate sf::Vector2i from SFML
// Graphics-agnostic 2D integer vector for window size and mouse position
struct Vec2i {
    int x;
    int y;
};

enum type {
    // Old types (removed, kept for reference):
    // KEY,           // Replaced by KEY_PRESSED/KEY_RELEASED
    // MOUSE,         // Replaced by MOUSE_BUTTON_PRESSED/MOUSE_BUTTON_RELEASED
    // EXIT,          // Replaced by CLOSED
    
    // Match EventManager's EventType enum
    CLOSED = 0,                    // Window closed (was EXIT)
    RESIZED = 1,                    // Window resized
    FOCUS_LOST = 2,                 // Window lost focus
    FOCUS_GAINED = 3,               // Window gained focus
    TEXT_ENTERED = 4,               // Text input
    KEY_PRESSED = 5,                // Key pressed (was KEY)
    KEY_RELEASED = 6,               // Key released
    MOUSE_WHEEL_SCROLLED = 7,       // Mouse wheel scrolled
    MOUSE_BUTTON_PRESSED = 8,       // Mouse button pressed (was MOUSE)
    MOUSE_BUTTON_RELEASED = 9,      // Mouse button released
    MOUSE_MOVED = 10,               // Mouse moved
    MOUSE_MOVED_RAW = 11,           // Mouse moved (raw)
    MOUSE_ENTERED = 12,             // Mouse entered window
    MOUSE_LEFT = 13,                // Mouse left window
    EMPTY = 99                      // No event (used by graphics libraries for polling)
};

typedef struct s_event {
    type type;
    union {
        int keyCode;        // When type == KEY_PRESSED/KEY_RELEASED: key code (UP, DOWN, LEFT, RIGHT, M, N, KEY_1, KEY_2, KEY_3)
        struct {
            int x, y;       // When type == MOUSE_*: mouse coordinates
            int button;     // When type == MOUSE_BUTTON_*: button code (0=left, 1=right, 2=middle)
        } mouse;
        struct {
            int width, height;  // When type == RESIZED: new window dimensions
        } window;
        int wheelDelta;     // When type == MOUSE_WHEEL_SCROLLED: scroll delta
        std::uint32_t unicode;  // When type == TEXT_ENTERED: unicode character
    };
} t_event;

typedef void *(*initFunc)(int, int, void *);
typedef void (*loopFunc)(void *);
typedef void (*beginFrameFunc)(void *);
typedef void (*endFrameFunc)(void *);
typedef void (*cleanupFunc)(void *);
typedef t_event (*checkEventsFunc)(void *);
typedef void (*loadAssetsFunc)(void *, const char **);
typedef void (*drawTextFunc)(void *, float, float, int, const char *);
typedef void (*drawAssetFunc)(void *, float, float, float, float, int, const char *);
typedef void (*drawButtonFunc)(void *, float, float, float, float, const char *);

#endif
