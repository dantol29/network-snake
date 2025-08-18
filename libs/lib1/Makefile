UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)   # macOS
	NAME     = lib1.dylib
	LDFLAGS  = -dynamiclib
	SYSLIBS  = -framework Cocoa -framework IOKit -framework CoreVideo \
	-framework CoreFoundation -framework Metal -framework QuartzCore \
	-framework OpenGL 
else                        # Linux
	NAME     = lib1.so
	LDFLAGS  = -shared
	SYSLIBS  = -ldl -lpthread
endif

CC          = c++
CFLAGS      = -O2 -Wall -Wextra -fPIC
SOURCES     = Graphics.cpp
OBJECTS     = $(SOURCES:.cpp=.o)

GLFW_DIR    = external/glfw
GLFW_BUILD  = $(GLFW_DIR)/build
GLFW_LIB    = $(GLFW_BUILD)/src/libglfw3.a
GLFW_INC    = -I$(GLFW_DIR)/include

all: $(NAME)

$(NAME): $(OBJECTS) $(GLFW_LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) $(GLFW_LIB) -Wl,-undefined,dynamic_lookup $(SYSLIBS) -o $@

%.o: %.cpp $(GLFW_LIB)
	$(CC) $(CFLAGS) $(GLFW_INC) -c $< -o $@

$(GLFW_LIB):
	git clone https://github.com/glfw/glfw.git $(GLFW_DIR) || true
	mkdir -p $(GLFW_BUILD)
	cd $(GLFW_BUILD) && cmake -DCMAKE_OSX_ARCHITECTURES=arm64 .. && make -j

clean:
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) $(NAME)
	$(RM) -rf ${GLFW_DIR}

re: fclean all

.PHONY: all clean fclean re
