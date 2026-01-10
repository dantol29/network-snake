UNAME_S := $(shell uname)

ifeq ($(UNAME_S),Darwin)        # macOS
    NAME     = lib2.dylib
    SOFLAGS  = -dynamiclib -undefined dynamic_lookup -Wl,-rpath,@loader_path
else                            # Linux
    NAME     = lib2.so
    SOFLAGS  = -shared -Wl,-rpath,'$$ORIGIN'
endif

SOFLAGS += -Wl,-rpath,'$$ORIGIN/SFML/lib'

CC      = g++
CFLAGS  = -std=c++17 -Wall -Wextra -Werror -O2 -fPIC

SOURCES = Graphics.cpp ../IGraphics.cpp
OBJECTS = $(SOURCES:.cpp=.o)

SFML_DIR        = SFML
SFML_BUILD      = $(SFML_DIR)/build
SFML_INSTALL    = $(SFML_DIR)
SFML_INC        = -I$(SFML_INSTALL)/include
SFML_LIBDIR     = $(SFML_INSTALL)/lib
SFML_LIBS       = -L$(SFML_LIBDIR) -lsfml-graphics -lsfml-window -lsfml-system
SFML_LIB_FILES = \
  $(SFML_LIBDIR)/libsfml-graphics.so \
  $(SFML_LIBDIR)/libsfml-window.so \
  $(SFML_LIBDIR)/libsfml-system.so

SFML_BUILT := $(SFML_LIBDIR)/.built

SFML_CMAKE_FLAGS = -DSFML_BUILD_NETWORK=OFF -DSFML_BUILD_EXAMPLES=OFF -DSFML_BUILD_DOC=OFF -DSFML_BUILD_AUDIO=OFF \
                   -DCMAKE_BUILD_TYPE=Release \
                   -DCMAKE_INSTALL_PREFIX=$(abspath $(SFML_INSTALL)) -DBUILD_SHARED_LIBS=ON

# Use centralized CMake if available, otherwise fall back to system cmake
CMAKE ?= cmake

all: $(NAME) copy_deps

$(NAME): $(OBJECTS) $(SFML_BUILT)
	$(CC) $(SOFLAGS) -o $@ $(OBJECTS) $(SFML_LIBS)

%.o: %.cpp $(SFML_BUILT)
	$(CC) $(CFLAGS) $(SFML_INC) -c $< -o $@

$(SFML_BUILT):
	git clone --depth=1 https://github.com/SFML/SFML.git $(SFML_DIR) || true
	mkdir -p $(SFML_BUILD) $(SFML_INSTALL)
	cd $(SFML_BUILD) && $(CMAKE) $(SFML_CMAKE_FLAGS) .. && $(MAKE) -j1 && $(MAKE) install
	touch $(SFML_BUILT)

copy_deps: $(SFML_BUILT)
ifeq ($(UNAME_S), Darwin)
	@cp $(SFML_LIBDIR)/libsfml-graphics*.dylib .
	@cp $(SFML_LIBDIR)/libsfml-window*.dylib .
	@cp $(SFML_LIBDIR)/libsfml-system*.dylib .
else
	@cp $(SFML_LIBDIR)/libsfml-graphics.so* .
	@cp $(SFML_LIBDIR)/libsfml-window.so* .
	@cp $(SFML_LIBDIR)/libsfml-system.so* .
endif

clean:
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) $(NAME)
	$(RM) -rf $(SFML_DIR)

re: fclean all

.PHONY: all clean fclean re
