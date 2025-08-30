#include "../includes/nibbler.hpp"
#include "Client.hpp"
#include "Drawer.hpp"

void onerror(const char *msg)
{
    write(STDERR_FILENO, msg, strlen(msg));
    write(2, "\n", 2);
    perror("exit");
    exit(EXIT_FAILURE);
}

int main()
{
    std::atomic<bool> stopFlag(false);
    Client *client = new Client(stopFlag);
    Drawer *drawer = new Drawer(client);

    drawer->loadDynamicLibrary("/Users/tolmadan/Desktop/42/nibbler/libs/lib1/lib1.dylib");

    std::thread clientThread(&Client::start, client);

    drawer->start();
    stopFlag.store(true);

    if (clientThread.joinable())
        clientThread.join();

    delete drawer;
    delete client;
}
