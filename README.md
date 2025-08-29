# Play classic snake online!

## How to play?
`./start.sh`

## Switch between 3 graphical libraries at runtime!

1. GLFW(OpenGL)
2. -
3. -


## TODO

- [] Replace onerror() with proper exit
- [] Add 2 dynamic libs
- [] Check memory leaks
- [x] Stop server when game is finished
- [x] Scale game based on width and height
- [x] Draw borders
- [x] Spawn more food if more players
- [] Add canonical form for classes
- [x] Accept both game width and height
- [x] Parse args - too big, too small
- [x] The edges of the plane can’t be passed through
- [] The snake starts with a size of 4 squares in the middle of the game area.
- [x] Single player mode must remain available
- [x] Add const
- [] If game field is not square - it is not rendered correctly
- [] Review potential data races
- [x] When player quits and rejoins - do not increase game field
- [x] Send snakes coordinates, so that client knows which snake to follow

## Features

- [x] Scale game field when new player is joining
- [x] Center snake on the screen
- [x] Zoom in & Zoom out
- [] Improve graphics - draw smooth snake position transactions before new data arrives

## If game scales - things to improve

- A hashmap of `unordered_map<int fd, snakes>` to remove iterations over snakes vector
- Add logic if data is not sent/received to socket in one write
- Send only map around the snake (in case field gets really big)

## Materials

1. https://www.cs.dartmouth.edu/~campbell/cs50/socketprogramming
