UNAME_S := $(shell uname)

ifeq ($(UNAME_S),Darwin)        # macOS
    NAME     = lib2.dylib
    SOFLAGS  = -dynamiclib -undefined dynamic_lookup
else                             # Linux
    NAME     = lib2.so
    SOFLAGS  = -shared
endif

CC = clang++
CFLAGS = -std=c++17 -Wall -Wextra -Werror -O2 -fPIC
SFML_CFLAGS := $(shell pkg-config --cflags sfml-graphics)
SFML_LIBS   := $(shell pkg-config --libs   sfml-graphics)

SOURCES  = Graphics.cpp
OBJECTS  = $(SOURCES:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(SOFLAGS) -o $@ $(OBJECTS) $(SFML_LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(SFML_CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
