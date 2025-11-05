# Linux-only: Even if apt fails gracefully on macOS, running sudo can cause root-owned files and permission errors during cleanup
if [[ "$(uname)" == "Linux" ]]; then
    sudo apt install \
        build-essential cmake git \
        libx11-dev libxrandr-dev libxcb1-dev libxcb-image0-dev libxcb-shm0-dev \
        libxcb-keysyms1-dev libxcb-icccm4-dev libxcb-util-dev libxcb-cursor-dev \
        libudev-dev libgl1-mesa-dev libopengl-dev \
        libfreetype6-dev libharfbuzz-dev \
        libjpeg-dev libflac-dev libogg-dev libvorbis-dev libvorbisenc2 libvorbisfile3 \
        libopenal-dev libpthread-stubs0-dev \
        libx11-xcb-dev
fi

{
(cd libs/lib1 && make re)
(cd libs/lib2 && make re)
(cd libs/lib4 && make re) || true
(cd client && make re)
(cd client && ./client)
} 2>&1 | tee log.txt
