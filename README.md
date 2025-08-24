# Play classic snake over SSH with your friends!

## Switch between 3 graphical libraries at runtime!

1. GLFW(OpenGL)
2. -
3. -

## Multi-threaded execution for the smoothest experience!

1. A thread for the game logic
2. A thread for the server
3. A thread for the client
4. A thread for the drawer

## TODO

- [] Replace onerror() with proper exit
- [] Add 2 dynamic libs
- [] Check memory leaks
- [x] Stop server when game is finished
- [x] Scale game based on width and height
- [] Draw borders
- [] Spawn more food if more players
- [] Add canonical form for classes
- [x] Accept both game width and height
- [x] Parse args - too big, too small
- [x] The edges of the plane can’t be passed through
- [] The snake starts with a size of 4 squares in the middle of the game area.
- [] Single player mode must remain available
- [x] Add const
- [] If game field is not square - it is not rendered correctly
- [] Review potential data races
- [x] When player quits and rejoins - do not increase game field

## Features

- [x] Scale game field when new player is joining
- [x] Center snake on the screen
- [] Improve graphics - draw smooth snake position transactions before new data arrives

## Materials

1. https://www.cs.dartmouth.edu/~campbell/cs50/socketprogramming
