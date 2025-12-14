#ifndef NIBBLER_HPP
#define NIBBLER_HPP

#include <atomic>
#include <fcntl.h>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <optional>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <time.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

#include "../../packet_generated.h"
#include <flatbuffers/flatbuffers.h>

#define FLOOR_SYMBOL '.'
#define SNAKE_SPEED 1000
#define MAX_PLAYERS 100

typedef struct s_coordinates {
  int x;
  int y;
} t_coordinates;

enum e_direction { UP, DOWN, LEFT, RIGHT };

#endif
