#include "../includes/nibbler.hpp"
#include "Game.hpp"
#include "Server.hpp"

void onerror(const char* msg) {
  write(STDERR_FILENO, msg, strlen(msg));
  exit(EXIT_FAILURE);
}

bool isValidNumber(const char* str) {
  if (!str || *str == '\0')
    return false;
  for (int i = 0; str[i]; i++) {
    if (str[i] < '0' || str[i] > '9')
      return false;
  }
  return true;
}

int main(int argc, char** argv) {
  if (argc < 3)
    onerror("Usage: ./nibbler_server height width [map]\n");

  if (!isValidNumber(argv[1]) || !isValidNumber(argv[2]))
    onerror("Error: Height and width must be positive numbers\n");

  int height = atoi(argv[1]);
  int width = atoi(argv[2]);
  
  if (height < 10 || width < 10)
    onerror("Error: Height and width must be at least 10\n");
  if (height > 100 || width > 100)
    onerror("Error: Height and width must not exceed 100\n");

  std::string mapPath = argc == 4 ? argv[3] : "";

  Game* game = new Game(height, width, mapPath);
  Server* server = new Server(game);

  std::thread gameThread(&Game::start, game);

  server->start();

  if (gameThread.joinable())
    gameThread.join();

  delete game;
  delete server;
}
