#ifndef DRAWER_HPP
#define DRAWER_HPP

#include "../includes/nibbler.hpp"
#include "Client.hpp"
#include "EventManager.hpp"

enum mode { MENU, GAME };

struct Button {
  float x;
  float y;
  float width;
  float height;
  std::string label;
  enum { MULTIPLAYER, SINGLE_PLAYER } type;
};

class Client;

class Drawer {
public:
  Drawer(Client* client);
  Drawer(const Drawer& obj) = delete;
  Drawer& operator=(const Drawer& obj) = delete;
  ~Drawer();

  void start();

private:
  Client* client;
  EventManager* eventManager;
  void* dynamicLibrary = nullptr;
  void* window = nullptr;
  int screenSize;
  int tilePx;
  int height;
  int width;
  int prevSnakeHeadX;
  int prevSnakeHeadY;
  bool gameRunning = true;
  std::string switchLibPath;
  std::thread clientThread;
  mode gameMode;
  std::vector<char*> assets;
  const Button multiplayerButton;
  const Button singlePlayerButton;
  std::pair<int, std::string> tailFrame;

  initFunc init;
  checkEventsFunc checkEvents;
  beginFrameFunc beginFrame;
  endFrameFunc endFrame;
  loadAssetsFunc loadAssets;
  drawAssetFunc drawAsset;
  drawButtonFunc drawButton;
  drawTextFunc drawText;
  cleanupFunc cleanup;

  void stopClient();
  void startClient(const std::string& serverIP, bool isSinglePlayer);
  void openWindow();
  void startDynamicLib();
  void closeDynamicLib();
  void onMouseUp(float x, float y);

  // EventManager callbacks
  void MoveUp(MatchedEventDetails* details);
  void MoveDown(MatchedEventDetails* details);
  void MoveLeft(MatchedEventDetails* details);
  void MoveRight(MatchedEventDetails* details);
  void ZoomIn(MatchedEventDetails* details);
  void ZoomOut(MatchedEventDetails* details);
  void SwitchLib1(MatchedEventDetails* details);
  void SwitchLib2(MatchedEventDetails* details);
  void SwitchLib3(MatchedEventDetails* details);
  void OnMouseClick(MatchedEventDetails* details);

  void loadDynamicLibrary(const std::string& lib);
  void drawBorder(int x, int y, int px, int py, int tilePx);
  void drawGameField();
  void drawMenu();
  void readAssets();
  void setTailFrame();
};

#endif
