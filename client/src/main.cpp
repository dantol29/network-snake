#include "Client.hpp"
#include "Drawer.hpp"

int main() {
  Client* client = new Client();
  Drawer* drawer = new Drawer(client);

  drawer->start();

  delete drawer;
  delete client;
}
