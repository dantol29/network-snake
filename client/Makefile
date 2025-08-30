NAME = client
CC = c++
CFLAGS = -O2 -Wall -Wextra -std=c++17 -pthread
FSANITIZE = -fsanitize=thread
SOURCES_M := src/main.cpp src/Client.cpp src/Drawer.cpp

OBJECTS := $(SOURCES_M:.cpp=.o)

$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(FSANITIZE) -o $(NAME)

all: $(NAME)

clean:
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
