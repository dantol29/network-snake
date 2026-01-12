NAME = nibbler_server
CC = c++
CFLAGS = -g -Wall -Wextra -Werror -std=c++17
FSANITIZE = -fsanitize=address

INCLUDES = -I./includes -I../flatbuffers/include

SOURCES_M := src/main.cpp src/Game.cpp src/Snake.cpp src/Server.cpp
OBJECTS := $(SOURCES_M:.cpp=.o)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Link
$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(NAME)

all: $(NAME)

clean:
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
