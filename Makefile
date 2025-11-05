# Top-level Makefile: Centralized CMake management
# All libraries require different CMake versions:
# - lib1 (raylib): CMake ≥ 3.25
# - lib2 (SFML 3.x): CMake ≥ 3.28
# - lib4 (SDL3): works with 3.22, but we use 3.29.6 for all

CMAKE_VERSION := 3.29.6
UNAME_S := $(shell uname)

# Platform-specific CMake paths
ifeq ($(UNAME_S),Darwin)
    CMAKE_HOME := $(HOME)/cmake-$(CMAKE_VERSION)-macos-universal
    CMAKE_ARCHIVE := cmake-$(CMAKE_VERSION)-macos-universal.tar.gz
else
    CMAKE_HOME := $(HOME)/cmake-$(CMAKE_VERSION)-linux-x86_64
    CMAKE_ARCHIVE := cmake-$(CMAKE_VERSION)-linux-x86_64.tar.gz
endif

CMAKE_BIN := $(CMAKE_HOME)/bin/cmake

# Per-lib CMake minimum versions (for reference/future-proofing)
LIB1_CMAKE_MIN := 3.25
LIB2_CMAKE_MIN := 3.28
LIB4_CMAKE_MIN := 3.22

# Bootstrap CMake if not already present
$(CMAKE_BIN):
	@echo "Bootstrapping CMake $(CMAKE_VERSION) for $(UNAME_S)..."
	@mkdir -p $(HOME)
	@if [ ! -d "$(CMAKE_HOME)" ]; then \
		echo "Downloading CMake $(CMAKE_VERSION)..."; \
		wget -q https://github.com/Kitware/CMake/releases/download/v$(CMAKE_VERSION)/$(CMAKE_ARCHIVE) && \
		tar -xzf $(CMAKE_ARCHIVE) -C $(HOME) && \
		rm -f $(CMAKE_ARCHIVE); \
	fi
	@echo "CMake $(CMAKE_VERSION) ready at $(CMAKE_BIN)"

# Export CMAKE for child Makefiles (will be used by lib Makefiles via CMAKE ?= cmake)
export CMAKE := $(CMAKE_BIN)

# Build all libraries
libs: $(CMAKE_BIN)
	@echo "Building all libraries..."
	$(MAKE) -C libs/lib1
	$(MAKE) -C libs/lib2
	$(MAKE) -C libs/lib4 || true

# Build client
client: $(CMAKE_BIN) libs
	$(MAKE) -C client

# Build server
server: $(CMAKE_BIN)
	$(MAKE) -C server

# Build everything (default target)
all: libs client server
	@echo "Build complete!"

# Run client (assumes server is running separately)
run: client
	@echo "Starting client..."
	cd client && ./client

# Run server (requires height and width arguments)
# Usage: make run-server HEIGHT=20 WIDTH=30
run-server: server
	@if [ -z "$(HEIGHT)" ] || [ -z "$(WIDTH)" ]; then \
		echo "Usage: make run-server HEIGHT=20 WIDTH=30"; \
		exit 1; \
	fi
	@echo "Starting server with size $(HEIGHT)x$(WIDTH)..."
	cd server && ./nibbler $(HEIGHT) $(WIDTH)

# Run both server and client together
# Usage: make run-game HEIGHT=20 WIDTH=30
run-game: server client
	@if [ -z "$(HEIGHT)" ] || [ -z "$(WIDTH)" ]; then \
		echo "Using defaults: HEIGHT=20 WIDTH=30"; \
		HEIGHT=20; WIDTH=30; \
	else \
		HEIGHT=$(HEIGHT); WIDTH=$(WIDTH); \
	fi
	@echo "Starting server with size $$HEIGHTx$$WIDTH..."
	@cd server && ./nibbler $$HEIGHT $$WIDTH & \
	SERVER_PID=$$!; \
	trap "kill $$SERVER_PID 2>/dev/null" EXIT INT TERM; \
	sleep 1; \
	echo "Starting client..."; \
	cd client && ./client || true; \
	kill $$SERVER_PID 2>/dev/null || true

# Run with valgrind to check for memory leaks
# Usage: make valgrind HEIGHT=20 WIDTH=30
valgrind: server client
	@if ! command -v valgrind >/dev/null 2>&1; then \
		echo "Error: valgrind is not installed. Install with: sudo apt install valgrind"; \
		exit 1; \
	fi
	@if [ -z "$(HEIGHT)" ] || [ -z "$(WIDTH)" ]; then \
		HEIGHT=20; WIDTH=30; \
		echo "Using defaults: HEIGHT=$$HEIGHT WIDTH=$$WIDTH"; \
	else \
		HEIGHT=$(HEIGHT); WIDTH=$(WIDTH); \
	fi; \
	echo "Starting server with valgrind (size $$HEIGHTx$$WIDTH)..."; \
	cd server && valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--log-file=../valgrind-server.log ./nibbler $$HEIGHT $$WIDTH & \
	SERVER_PID=$$!; \
	trap "kill $$SERVER_PID 2>/dev/null" EXIT INT TERM; \
	sleep 2; \
	echo "Starting client with valgrind..."; \
	cd client && valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--log-file=valgrind-client.log ./client || true; \
	kill $$SERVER_PID 2>/dev/null || true; \
	echo "Valgrind logs saved to valgrind-server.log and valgrind-client.log"

# Clean all build artifacts
clean:
	$(MAKE) -C libs/lib1 clean || true
	$(MAKE) -C libs/lib2 clean || true
	$(MAKE) -C libs/lib4 clean || true
	$(MAKE) -C client clean || true
	$(MAKE) -C server clean || true

# Full clean (removes all build artifacts and dependencies)
fclean: clean
	$(MAKE) -C libs/lib1 fclean || true
	$(MAKE) -C libs/lib2 fclean || true
	$(MAKE) -C libs/lib4 fclean || true
	$(MAKE) -C client fclean || true
	$(MAKE) -C server fclean || true

# Rebuild everything
re: fclean all

.PHONY: all libs client server run run-server run-game valgrind clean fclean re

