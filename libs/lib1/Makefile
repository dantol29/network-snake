ifeq ($(shell uname), Darwin)    # macOS
    NAME     = lib1.dylib
    LDFLAGS  = -shared -undefined dynamic_lookup
    SYSLIBS  = -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL
    RAYLIB_CMAKE_FLAGS =
else                             # Linux
    NAME     = lib1.so
    LDFLAGS  = -shared
    SYSLIBS  = -ldl -lpthread -lGL -lX11 -lXrandr -lXinerama -lXcursor -lXi
    RAYLIB_CMAKE_FLAGS = -DPLATFORM=Desktop -DGLFW_BUILD_WAYLAND=OFF
endif

CC          = c++
CFLAGS      = -O2 -Wall -Wextra -fPIC -std=c++11
SOURCES     = Graphics.cpp
OBJECTS     = $(SOURCES:.cpp=.o)

# raylib paths
RAYLIB_DIR   = raylib
RAYLIB_BUILD = $(RAYLIB_DIR)/build
RAYLIB_INC   = -I$(RAYLIB_DIR)/src
RAYLIB_LIBA  = $(RAYLIB_BUILD)/raylib/libraylib.a

all: $(NAME)

$(NAME): $(OBJECTS) $(RAYLIB_LIBA)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) $(RAYLIB_LIBA) $(SYSLIBS) -o $@

%.o: %.cpp $(RAYLIB_LIBA)
	$(CC) $(CFLAGS) $(RAYLIB_INC) -c $< -o $@ 

$(RAYLIB_LIBA):
	git clone https://github.com/raysan5/raylib.git $(RAYLIB_DIR) || true
	mkdir -p $(RAYLIB_BUILD)                                              
	cd $(RAYLIB_BUILD) && cmake -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DBUILD_EXAMPLES=OFF -DBUILD_GAMES=OFF $(RAYLIB_CMAKE_FLAGS) .. && $(MAKE) -j

clean:
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) $(NAME)
	$(RM) -rf $(RAYLIB_DIR)

re: fclean
	$(MAKE) all

.PHONY: all clean fclean re
