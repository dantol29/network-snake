UNAME_S := $(shell uname)

NAME = nibbler_client
CC = c++
CFLAGS = -g -O0 -Wall -Wextra -Werror -std=c++17 -pthread
FSANITIZE = -fsanitize=thread
SOURCES_M := src/main.cpp src/AnimationManager.cpp src/Client.cpp src/Drawer.cpp src/EventManager.cpp

INCLUDES = -I./includes -I../flatbuffers/include

OBJECTS := $(SOURCES_M:.cpp=.o)

# Platform-specific linker flags
ifeq ($(UNAME_S),Darwin)
    # macOS: libraries use -undefined dynamic_lookup, so no -rdynamic needed here
    # dlopen/dlsym are in system libraries, no -ldl needed
    LDFLAGS =
else
    # Linux: need -rdynamic to export symbols for dynamically loaded libraries
    # dlopen/dlsym require -ldl
    LDFLAGS = -rdynamic -ldl
endif

$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(NAME)

# Debug build with thread sanitizer (for testing only, not compatible with OpenGL)
sanitize: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) $(FSANITIZE) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

all: $(NAME)

clean:
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
