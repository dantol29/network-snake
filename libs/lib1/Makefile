ifeq ($(shell uname), Darwin)    # macOS
        NAME     = lib1.dylib
        LDFLAGS  = -shared -undefined dynamic_lookup
        SYSLIBS  = -framework Cocoa -framework IOKit -framework CoreVideo
        GLFW_CMAKE_FLAGS = -DGLFW_USE_WAYLAND=OFF -DGLFW_USE_X11=OFF
else                            # Linux
        NAME     = lib1.so
        LDFLAGS  = -shared
        SYSLIBS  = -ldl -lpthread -lGL -lX11 -lXrandr -lXinerama -lXcursor -lXi
        GLFW_CMAKE_FLAGS =
endif

CC          = c++
CFLAGS      = -O2 -Wall -Wextra -fPIC
SOURCES     = Graphics.cpp
OBJECTS     = $(SOURCES:.cpp=.o)

GLFW_DIR    = GLFW
GLFW_BUILD  = $(GLFW_DIR)/build
GLFW_LIB    = $(GLFW_BUILD)/src/libglfw3.a
GLFW_INC    = -I$(GLFW_DIR)/include

all: $(NAME)

$(NAME): $(OBJECTS) $(GLFW_LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) $(GLFW_LIB) $(SYSLIBS) -o $@

%.o: %.cpp $(GLFW_LIB)
	$(CC) $(CFLAGS) $(GLFW_INC) -c $< -o $@

$(GLFW_LIB):
	git clone https://github.com/glfw/glfw.git $(GLFW_DIR) || true
	mkdir -p $(GLFW_BUILD)
	cd $(GLFW_BUILD) && cmake $(GLFW_CMAKE_FLAGS) .. && make -j

clean:
	$(RM) $(OBJECTS)
	$(RM) -rf $(GLFW_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean
	$(RM) -rf $(GLFW_DIR)
	$(MAKE) all

.PHONY: all clean fclean re
