NAME = nibbler
CC = c++
CFLAGS = -g -Wall -Wextra -Werror -std=c++17
FSANITIZE = -fsanitize=address
SOURCES_M := src/main.cpp src/Game.cpp src/Snake.cpp src/Server.cpp

OBJECTS := $(SOURCES_M:.cpp=.o)

$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(NAME)

all: $(NAME)

clean:
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
