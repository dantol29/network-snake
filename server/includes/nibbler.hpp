#ifndef NIBBLER_HPP
#define NIBBLER_HPP

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <string.h>
#include <poll.h>
#include <fcntl.h>
#include <vector>
#include <iostream>
#include <thread>
#include <atomic>
#include <memory>
#include <mutex>

#define FLOOR_SYMBOL '.'
#define SNAKE_SPEED 150

struct snake
{
    int x;
    int y;
    snake *next;
    snake *prev;
};

enum e_direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// utils
[[noreturn]] void onerror(const char *msg);

#endif
