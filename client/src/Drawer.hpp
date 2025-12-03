#ifndef DRAWER_HPP
#define DRAWER_HPP

#include "../includes/nibbler.hpp"
#include "Client.hpp"
#include "EventManager.hpp"

enum mode
{
    MENU,
    GAME
};

struct Button {
    float x;
    float y;
    float width;
    float height;
    std::string label;
    enum { MULTIPLAYER, SINGLE_PLAYER } type;
};

class Client;

class Drawer
{
public:
    Drawer(Client *client);
    Drawer(const Drawer &obj) = delete;
    Drawer& operator=(const Drawer& obj) = delete;
    ~Drawer();

    void start();

private:
    Client *client;
    EventManager *eventManager;
    void *dynamicLibrary = nullptr;
    void *window = nullptr;
    int screenSize;
    int tilePx;
    int height;
    int width;
    int prevSnakeHeadX;
    int prevSnakeHeadY;
    bool gameRunning = true;
    bool isMenuDrawn = false;
    std::string switchLibPath;
    std::thread clientThread;
    mode gameMode;
    struct rgb rgb;
    const Button multiplayerButton;
    const Button singlePlayerButton;
    
    initFunc init;
    checkEventsFunc checkEvents;
    beginFrameFunc beginFrame;
    endFrameFunc endFrame;
    setShouldUpdateScreenFunc setShouldUpdateScreen;
    drawSquareFunc drawSquare;
    drawButtonFunc drawButton;
    drawTextFunc drawText;
    cleanupFunc cleanup;
    
    void stopClient();
    void openWindow();
    void startDynamicLib();
    void closeDynamicLib();
    void onMouseUp(float x, float y);
    // Old onKeyPress - kept for reference, replaced by EventManager callbacks
    // void onKeyPress(int action);
    
    // EventManager callbacks
    void MoveUp(EventDetails* l_details);
    void MoveDown(EventDetails* l_details);
    void MoveLeft(EventDetails* l_details);
    void MoveRight(EventDetails* l_details);
    void ZoomIn(EventDetails* l_details);
    void ZoomOut(EventDetails* l_details);
    void SwitchLib1(EventDetails* l_details);
    void SwitchLib2(EventDetails* l_details);
    void SwitchLib3(EventDetails* l_details);
    
    void loadDynamicLibrary(const std::string &lib);
    void drawBorder(int x, int y, int px, int py, int tilePx);
    void drawGameField();
    void drawMenu();
};

#endif
