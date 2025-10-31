UNAME_S := $(shell uname)

ifeq ($(UNAME_S),Darwin)        # macOS
    NAME     = lib2.dylib
    SOFLAGS  = -dynamiclib -undefined dynamic_lookup -Wl,-rpath,@loader_path
else                            # Linux
    NAME     = lib2.so
    SOFLAGS  = -shared -Wl,-rpath,'$$ORIGIN'
endif

CC      = g++
CFLAGS  = -std=c++17 -Wall -Wextra -Werror -O2 -fPIC

SOURCES = Graphics.cpp
OBJECTS = $(SOURCES:.cpp=.o)

SFML_DIR        = SFML
SFML_BUILD      = $(SFML_DIR)/build
SFML_INSTALL    = $(SFML_DIR)
SFML_INC        = -I$(SFML_INSTALL)/include
SFML_LIBDIR     = $(SFML_INSTALL)/lib
SFML_LIBS       = -L$(SFML_LIBDIR) -lsfml-graphics -lsfml-window -lsfml-system
SFML_CMAKE_FLAGS = -DSFML_BUILD_EXAMPLES=OFF -DSFML_BUILD_DOC=OFF -DCMAKE_BUILD_TYPE=Release \
                   -DCMAKE_INSTALL_PREFIX=$(abspath $(SFML_INSTALL)) -DBUILD_SHARED_LIBS=ON

all: $(NAME) copy_deps

$(NAME): $(OBJECTS) $(SFML_INSTALL)
	$(CC) $(SOFLAGS) -o $@ $(OBJECTS) $(SFML_LIBS)

%.o: %.cpp $(SFML_INSTALL)
	$(CC) $(CFLAGS) $(SFML_INC) -c $< -o $@

$(SFML_INSTALL):
	git clone https://github.com/SFML/SFML.git $(SFML_DIR) || true
	mkdir -p $(SFML_BUILD) $(SFML_INSTALL)
	cd $(SFML_BUILD) && cmake $(SFML_CMAKE_FLAGS) .. && $(MAKE) -j1 && $(MAKE) install

copy_deps:
ifeq ($(UNAME_S), Darwin)
	@cp $(SFML_LIBDIR)/libsfml-graphics.3.1.dylib .
	@cp $(SFML_LIBDIR)/libsfml-window.3.1.dylib .
	@cp $(SFML_LIBDIR)/libsfml-system.3.1.dylib .
else
	@cp $(SFML_LIBDIR)/libsfml-graphics.3.1.so .
	@cp $(SFML_LIBDIR)/libsfml-window.3.1.so .
	@cp $(SFML_LIBDIR)/libsfml-system.3.1.so .
endif

clean:
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) $(NAME)
	$(RM) -rf $(SFML_DIR)

re: fclean all

.PHONY: all clean fclean re
