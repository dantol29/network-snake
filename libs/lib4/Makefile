UNAME_S := $(shell uname)

ifeq ($(UNAME_S),Darwin)
    NAME     = lib3.dylib
    SOFLAGS  = -dynamiclib -undefined dynamic_lookup -Wl,-rpath,@loader_path
else
    NAME     = lib3.so
    SOFLAGS  = -shared -Wl,-rpath,'$$ORIGIN'
endif

CC      = g++
CFLAGS  = -std=c++17 -Wall -Wextra -Werror -O2 -fPIC

SOURCES = Graphics.cpp
OBJECTS = $(SOURCES:.cpp=.o)

SDL_INSTALL       = SDL3
SDL_CORE_DIR      = $(SDL_INSTALL)/SDL3
SDL_TTF_DIR       = $(SDL_INSTALL)/SDL_ttf
SDL_IMAGE_DIR     = $(SDL_INSTALL)/SDL_image

SDL_BUILD_CORE    = $(SDL_CORE_DIR)/build
SDL_BUILD_TTF     = $(SDL_TTF_DIR)/build
SDL_BUILD_IMAGE   = $(SDL_IMAGE_DIR)/build

SDL_INC        = -I$(SDL_INSTALL)/include
SDL_LIBDIR     = $(SDL_INSTALL)/lib
SDL_LIBS       = -L$(SDL_LIBDIR) -lSDL3 -lSDL3_ttf -lSDL3_image

SDL3_CMAKE_FLAGS = -DSDL_STATIC=OFF -DSDL_SHARED=ON -DSDL_TEST=OFF \
                   -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$(abspath $(SDL_INSTALL))

all: $(NAME)

# Build final library
$(NAME): $(OBJECTS)
	$(CC) $(SOFLAGS) -o $@ $(OBJECTS) $(SDL_LIBS)

# Compile objects only after all SDL headers/libs exist
%.o: %.cpp $(SDL_LIBDIR)/libSDL3.so $(SDL_LIBDIR)/libSDL3_ttf.so $(SDL_LIBDIR)/libSDL3_image.so
	$(CC) $(CFLAGS) $(SDL_INC) -c $< -o $@

# Build SDL3 core first
$(SDL_LIBDIR)/libSDL3.so:
	@if [ ! -d "$(SDL_CORE_DIR)" ]; then git clone https://github.com/libsdl-org/SDL.git $(SDL_CORE_DIR); fi
	@mkdir -p $(SDL_BUILD_CORE)
	@cd $(SDL_BUILD_CORE) && cmake $(SDL3_CMAKE_FLAGS) .. && $(MAKE) -j1 && $(MAKE) install

# Build SDL3_ttf (depends on SDL3 core)
$(SDL_LIBDIR)/libSDL3_ttf.so: $(SDL_LIBDIR)/libSDL3.so
	@if [ ! -d "$(SDL_TTF_DIR)" ]; then git clone https://github.com/libsdl-org/SDL_ttf.git $(SDL_TTF_DIR); fi
	@mkdir -p $(SDL_BUILD_TTF)
	@cd $(SDL_BUILD_TTF) && cmake -DSDL3TTF_SHARED=ON -DCMAKE_INSTALL_PREFIX=$(abspath $(SDL_INSTALL)) .. && $(MAKE) -j1 && $(MAKE) install

# Build SDL3_image (depends on SDL3 core)
$(SDL_LIBDIR)/libSDL3_image.so: $(SDL_LIBDIR)/libSDL3.so
	@if [ ! -d "$(SDL_IMAGE_DIR)" ]; then git clone https://github.com/libsdl-org/SDL_image.git $(SDL_IMAGE_DIR); fi
	@mkdir -p $(SDL_BUILD_IMAGE)
	@cd $(SDL_BUILD_IMAGE) && cmake -DSDL3IMAGE_SHARED=ON -DCMAKE_INSTALL_PREFIX=$(abspath $(SDL_INSTALL)) .. && $(MAKE) -j1 && $(MAKE) install

clean:
	rm -f $(OBJECTS) $(NAME)
	rm -rf $(SDL_BUILD_CORE) $(SDL_BUILD_TTF) $(SDL_BUILD_IMAGE)

