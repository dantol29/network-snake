# Linux-only: Even if apt fails gracefully on macOS, running sudo can cause root-owned files and permission errors during cleanup
if [[ "$(uname)" == "Linux" ]]; then
    sudo apt install \
        build-essential cmake git \
        libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxtst-dev libxcb1-dev libxcb-image0-dev libxcb-shm0-dev \
        libxcb-keysyms1-dev libxcb-icccm4-dev libxcb-util-dev libxcb-cursor-dev \
        libudev-dev libgl1-mesa-dev libopengl-dev \
        libfreetype6-dev libharfbuzz-dev \
        libjpeg-dev libflac-dev libogg-dev libvorbis-dev libvorbisenc2 libvorbisfile3 \
        libopenal-dev libpthread-stubs0-dev \
        libx11-xcb-dev \
        libasound2-dev libdbus-1-dev libwayland-dev libxss-dev libxkbcommon-dev \
        valgrind
fi

{
# Install system dependencies (Linux only)
# Then build everything and start server + client using the top-level Makefile
make all
make run-game HEIGHT=20 WIDTH=30
} 2>&1 | tee log.txt
