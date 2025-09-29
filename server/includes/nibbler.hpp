#ifndef NIBBLER_HPP
#define NIBBLER_HPP

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
#include <string>
#include <unordered_map>
#include <list>

#define FLOOR_SYMBOL '.'
#define SNAKE_SPEED 150
#define MAX_PLAYERS 100

struct coordinates
{
    int x;
    int y;
};

enum e_direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

#endif
