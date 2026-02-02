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

constexpr char FLOOR_TILE = '.';
constexpr char BODY_TILE = 'B';
constexpr char HEAD_TILE = 'H';
constexpr char TAIL_TILE = 'T';
constexpr char FOOD_TILE = 'F';
constexpr char WALL_HORIZ_TILE = 'W';
constexpr char WALL_VERTI_TILE = 'V';
constexpr int SNAKE_SPEED = 300;
constexpr int MAX_PLAYERS = 10;

typedef struct s_coordinates {
  int x;
  int y;
} t_coordinates;

enum class e_direction { UP, DOWN, LEFT, RIGHT };

#endif
