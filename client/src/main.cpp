#include "Client.hpp"
#include "Drawer.hpp"

int main() {
  auto client = std::make_shared<Client>();
  auto drawer = std::make_unique<Drawer>(client);

  drawer->start();
}
