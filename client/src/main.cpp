#include "../includes/nibbler.hpp"
#include "Client.hpp"
#include "Drawer.hpp"

int main()
{
    std::atomic<bool> stopFlag(false);
    Client *client = new Client(stopFlag);
    Drawer *drawer = new Drawer(client);

    std::thread clientThread(&Client::start, client);

    try
    {
        drawer->loadDynamicLibrary("/Users/tolmadan/Desktop/42/nibbler/libs/lib1/lib1");
        drawer->start();
    }
    catch (const char *msg)
    {
        std::cerr << msg << std::endl;
    }

    stopFlag.store(true);

    if (clientThread.joinable())
        clientThread.join();

    delete drawer;
    delete client;
}
