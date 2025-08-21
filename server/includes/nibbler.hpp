#ifndef NIBBLER_HPP
#define NIBBLER_HPP

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <poll.h>
#include <fcntl.h>
#include <vector>
#include <iostream>
#include <thread>

#define FLOOR_SYMBOL '.'

// utils
[[noreturn]] void onerror(const char *msg);

#endif
