sudo apt install build-essential libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev libglfw3-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
cd libs/lib1 && make re
cd libs/lib2 && make re
cd libs/lib3 && make re
cd ../../client && make re
./client
