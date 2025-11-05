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

# Default target: ensure CMake is available
all: $(CMAKE_BIN)
	@echo "CMake $(CMAKE_VERSION) is available at $(CMAKE_BIN)"

# Build all libraries
libs: $(CMAKE_BIN)
	@echo "Building all libraries..."
	$(MAKE) -C libs/lib1 re
	$(MAKE) -C libs/lib2 re
	$(MAKE) -C libs/lib4 re || true

# Build client
client: $(CMAKE_BIN)
	$(MAKE) -C client re

# Build server
server: $(CMAKE_BIN)
	$(MAKE) -C server re

# Build everything
build: libs client server

.PHONY: all libs client server build

