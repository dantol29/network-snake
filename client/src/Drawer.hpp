#ifndef DRAWER_HPP
#define DRAWER_HPP

#include "../includes/nibbler.hpp"
#include "Client.hpp"
#include "EventManager.hpp"
#include "AnimationManager.hpp"

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
  Drawer(Drawer&& obj) = delete;
  Drawer& operator=(Drawer&& obj) = delete;
  ~Drawer();

  void start();

private:
  Client* client;
  EventManager* eventManager;
  AnimationManager* animationManager;

  std::vector<char*> assets;
  void* dynamicLibrary = nullptr;
  void* window = nullptr;
  int tileSize;
  bool gameRunning = true;
  std::string switchLibPath;
  std::thread clientThread;
  const Button multiplayerButton;
  const Button singlePlayerButton;

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
  void loadDynamicLibrary(const std::string& lib);
  void readAssets();
  void drawGame();
  void drawMenu();
  void drawUI(const GameData* gameData, int playerId);
  void drawSnakes(const GameData* gameData);
  void drawFood(const GameData* gameData);
  void drawMap(const MapData* mapData);
  int getRotation(int x, int y, int x2, int y2) const;
  int cornerPartRotation(int x, int y, int x2, int y2) const;
  std::pair<std::string, int> getWallTexture(int x, int y, const MapData* mapData);

  // EventManager callbacks
  void MoveUp(t_event* details);
  void MoveDown(t_event* details);
  void MoveLeft(t_event* details);
  void MoveRight(t_event* details);
  void ZoomIn(t_event* details);
  void ZoomOut(t_event* details);
  void SwitchLib1(t_event* details);
  void SwitchLib2(t_event* details);
  void SwitchLib3(t_event* details);
  void OnMouseClick(t_event* details);
};

#endif
