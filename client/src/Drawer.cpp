#include "Drawer.hpp"
#include "EventManager.hpp"
#include <fstream>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000

Drawer::Drawer(Client* client)
    : client(client), switchLibPath("../libs/lib3/lib3"),
      multiplayerButton{400, 300, 200, 60, "Multiplayer", Button::MULTIPLAYER},
      singlePlayerButton{400, 400, 200, 60, "Single-player", Button::SINGLE_PLAYER} {
  tileSize = SCREEN_HEIGHT / 40;
  tailFrame = std::make_pair(1, "assets/tail.png");
  readAssets();

  eventManager = new EventManager();
  eventManager->AddCallback(StateType::Game, "Key_Up", &Drawer::MoveUp, this);
  eventManager->AddCallback(StateType::Game, "Key_Down", &Drawer::MoveDown, this);
  eventManager->AddCallback(StateType::Game, "Key_Left", &Drawer::MoveLeft, this);
  eventManager->AddCallback(StateType::Game, "Key_Right", &Drawer::MoveRight, this);
  eventManager->AddCallback(StateType::Game, "Key_Down", &Drawer::MoveDown, this);
  eventManager->AddCallback(StateType::Game, "Key_Left", &Drawer::MoveLeft, this);
  eventManager->AddCallback(StateType::Game, "Key_Right", &Drawer::MoveRight, this);
  eventManager->AddCallback(StateType::Game, "Key_W", &Drawer::MoveUp, this);
  eventManager->AddCallback(StateType::Game, "Key_A", &Drawer::MoveLeft, this);
  eventManager->AddCallback(StateType::Game, "Key_S", &Drawer::MoveDown, this);
  eventManager->AddCallback(StateType::Game, "Key_D", &Drawer::MoveRight, this);
  eventManager->AddCallback(StateType::Game, "Key_M", &Drawer::ZoomIn, this);
  eventManager->AddCallback(StateType::Game, "Key_N", &Drawer::ZoomOut, this);
  eventManager->AddCallback(StateType::Game, "Key_1", &Drawer::SwitchLib1, this);
  eventManager->AddCallback(StateType::Game, "Key_2", &Drawer::SwitchLib2, this);
  eventManager->AddCallback(StateType::Game, "Key_3", &Drawer::SwitchLib3, this);
  eventManager->AddCallback(StateType::Menu, "Mouse_Left", &Drawer::OnMouseClick, this);

  eventManager->SetCurrentState(StateType::Menu);
}

Drawer::~Drawer() {
  delete eventManager;
  this->stopClient();
  this->closeDynamicLib();

  for (int i = 0; assets[i]; ++i)
    free(assets[i]);

  std::cout << "Drawer destructor" << std::endl;
}

void Drawer::loadDynamicLibrary(const std::string& lib) {
  std::string libPath = lib + LIB_EXTENSION;
  this->dynamicLibrary = dlopen(libPath.c_str(), RTLD_LAZY);
  if (!this->dynamicLibrary)
    throw "Failed to load dynlib";

  dlerror(); // clean errors

  this->init = (initFunc)dlsym(this->dynamicLibrary, "init");
  this->cleanup = (cleanupFunc)dlsym(this->dynamicLibrary, "cleanup");
  this->loadAssets = (loadAssetsFunc)dlsym(this->dynamicLibrary, "loadAssets");
  this->drawAsset = (drawAssetFunc)dlsym(this->dynamicLibrary, "drawAsset");
  this->drawButton = (drawButtonFunc)dlsym(this->dynamicLibrary, "drawButton");
  this->drawText = (drawTextFunc)dlsym(this->dynamicLibrary, "drawText");
  this->beginFrame = (beginFrameFunc)dlsym(this->dynamicLibrary, "beginFrame");
  this->endFrame = (endFrameFunc)dlsym(this->dynamicLibrary, "endFrame");
  this->checkEvents = (checkEventsFunc)dlsym(this->dynamicLibrary, "checkEvents");

  char* error = dlerror(); // check dlsym calls
  if (error != NULL)
    throw "Failed to find functions in dynlib";

  if (!this->init || !this->cleanup || !this->drawAsset || !this->drawButton || !this->drawText ||
      !this->loadAssets || !this->endFrame || !this->beginFrame || !this->checkEvents)
    throw "Failed to init dynlib functions";
}

void Drawer::closeDynamicLib() {
  if (this->window) {
    this->cleanup(this->window);
    this->window = nullptr;
  }
  if (this->dynamicLibrary) {
	std::cout << "Closing dynamic lib" << std::endl;
    dlclose(this->dynamicLibrary);
    this->dynamicLibrary = nullptr;
  }
}

void Drawer::startDynamicLib() {
  this->closeDynamicLib();
  this->loadDynamicLibrary(this->switchLibPath);
  this->switchLibPath = "";
}

void Drawer::start() {
  try {
    while (1) {
      this->startDynamicLib();
      this->openWindow();
      this->loadAssets(this->window, (const char**)assets.data());
      gameRunning = true;

      while (gameRunning) {
        this->beginFrame(this->window);

        t_event event = this->checkEvents(this->window);
        if (event.type == CLOSED) {
			gameRunning = false;
			break;
		}
        else if (event.type != EMPTY) {
          eventManager->HandleEvent(event);
          eventManager->Update();
        }

        const StateType state = eventManager->getCurrentState();
        if (state == StateType::Game)
          this->drawGame();
        else if (state == StateType::Menu)
          this->drawMenu();

        this->endFrame(this->window);
      }

      if (this->switchLibPath.empty())
        break;
    }
  } catch (const char* msg) {
    std::cerr << msg << ": " << std::endl;
  }

  std::cout << "Exiting drawer" << std::endl;
  stopClient();
}

void Drawer::readAssets() {
  std::ifstream file("assets.cfg");
  if (!file.is_open())
    throw "Error opening assets file";

  std::string line;
  while (getline(file, line)) {
    assets.push_back(strdup(line.c_str()));
    line = "";
  }

  assets.push_back(nullptr);

  if (!file.eof())
    throw "Error reading assets file";

  file.close();
}

void Drawer::openWindow() {
  this->window = this->init(SCREEN_HEIGHT, SCREEN_WIDTH, this);
  if (!this->window)
    throw("Failed to init lib");
}

void Drawer::drawMenu() {
  this->drawText(this->window, 380, 200, 40, "42 SNAKES");
  this->drawButton(this->window, this->multiplayerButton.x, this->multiplayerButton.y,
                   this->multiplayerButton.width, this->multiplayerButton.height,
                   this->multiplayerButton.label.c_str());
  this->drawButton(this->window, this->singlePlayerButton.x, this->singlePlayerButton.y,
                   this->singlePlayerButton.width, this->singlePlayerButton.height,
                   this->singlePlayerButton.label.c_str());
}

void Drawer::drawUI(const GameData* gameData, int playerId) {
  this->drawText(this->window, 910, 10, 20, "SCORES");

  int height = 40;
  for (auto it = gameData->snakes()->begin(); it != gameData->snakes()->end(); ++it) {
    std::string playerName = it->id() == playerId ? "ME" : std::to_string(it->id());
    std::string displayText = playerName + ": " + std::to_string(it->score());
    this->drawText(this->window, 910, height, 20, displayText.c_str());

    height += 25;
  }

  this->drawText(this->window, 800, 775, 20, "M - ZOOM OUT");
  this->drawText(this->window, 800, 800, 20, "N - ZOOM IN");
}

void Drawer::drawGame() {
  if (client->getStopFlag())
    return stopClient();

  bool isPlayerAlive = false;

  {
    std::mutex& gameDataMutex = client->getGameDataMutex();
    std::mutex& mapDataMutex = client->getMapDataMutex();
    std::lock_guard<std::mutex> lock(gameDataMutex);
    std::lock_guard<std::mutex> lock2(mapDataMutex);

    const GameData* gameData = client->getGameData();
    if (!gameData)
      return;

    const MapData* mapData = client->getMapData();
    if (!mapData)
      return;

    int playerId = mapData->player_id();

    drawMap(mapData);
    drawFood(gameData);
    drawSnakes(gameData);
    drawUI(gameData, playerId);
    setTailFrame();

    for (auto it = gameData->snakes()->begin(); it != gameData->snakes()->end(); ++it) {
      if (it->id() == playerId) {
        isPlayerAlive = true;
        break;
      }
    }
  }

  if (!isPlayerAlive)
    stopClient();
}

void Drawer::drawSnakes(const GameData* gameData) {
  int rotation = 0;
  auto snakes = gameData->snakes();

  for (auto snake = snakes->begin(); snake != snakes->end(); ++snake) {
    auto body = snake->body();
    for (auto part = body->begin(); part != body->end(); ++part) {
      auto nextPart = part + 1;
      if (nextPart != body->end())
        rotation = getRotation(part->x(), part->y(), nextPart->x(), nextPart->y());
      else
        rotation = getRotation((part - 1)->x(), (part - 1)->y(), part->x(), part->y());

      std::string texture = "assets/body.png";
      if (part == body->begin())
        texture = "assets/head.png";
      else if (nextPart == body->end())
        texture = tailFrame.second;
      else {
        int cr = cornerPartRotation((part - 1)->x(), (part - 1)->y(), nextPart->x(), nextPart->y());
        if (cr) {
          texture = "assets/body_corner.png";

          // rotation should always be 90C on corners
          if (cr - rotation != 90)
            cr += 180;
          rotation = cr;
        }
      }

      // pixel on the screen to draw + offset(walls)
      int px = part->x() * tileSize + tileSize;
      int py = part->y() * tileSize + tileSize;
      this->drawAsset(this->window, px, py, tileSize, tileSize, rotation, texture.c_str());
    }
  }
}

void Drawer::drawFood(const GameData* gameData) {
  auto food = gameData->food();

  for (auto it = food->begin(); it != food->end(); ++it) {
    int px = it->x() * tileSize + tileSize;
    int py = it->y() * tileSize + tileSize;
    this->drawAsset(this->window, px, py, tileSize, tileSize, 0, "assets/food.png");
  }
}

void Drawer::drawMap(const MapData* mapData) {
  auto map = mapData->map();
  int y = 0;
  for (auto row = map->begin(); row != map->end(); ++row, ++y) {
    int x = 0;
    for (auto tile = row->row()->begin(); tile != row->row()->end(); ++tile, ++x) {
      int px = x * tileSize + tileSize;
      int py = y * tileSize + tileSize;

      if (x == 0)
        this->drawAsset(this->window, px - tileSize, py, tileSize, tileSize, 180, "assets/border.png");
      else if (x == (int)map->Get(y)->row()->size() - 1)
        this->drawAsset(this->window, px + tileSize, py, tileSize, tileSize, 0, "assets/border.png");

      if (y == 0)
        this->drawAsset(this->window, px, py - tileSize, tileSize, tileSize, 270, "assets/border.png");
      else if (y == (int)map->size() - 1)
        this->drawAsset(this->window, px, py + tileSize, tileSize, tileSize, 90, "assets/border.png");

      switch (*tile) {
      case Tile_WallHorizontal:
      case Tile_WallVertical: {
        auto wall = getWallTexture(x, y, mapData);
        this->drawAsset(this->window, px, py, tileSize, tileSize, wall.second, wall.first.c_str());
        break;
      }
      case Tile_Empty:
      default:
        continue;
      }
    }
  }
}

int Drawer::getRotation(int x, int y, int x2, int y2) const {
  int diff_x = x - x2;
  int diff_y = y - y2;
  if (diff_y < 0)
    return 270; // below
  if (diff_y > 0)
    return 90; // above
  if (diff_x < 0)
    return 180; // left
  if (diff_x > 0)
    return 0; // right

  return 0;
}

int Drawer::cornerPartRotation(int x, int y, int x2, int y2) const {
  int diff_x = x - x2;
  int diff_y = y - y2;
  if (diff_y > 0 && diff_x > 0)
    return 90; // lower-right
  if (diff_y > 0 && diff_x < 0)
    return 180; // lower-left
  if (diff_y < 0 && diff_x < 0)
    return 270; // upper-left
  if (diff_y < 0 && diff_x > 0)
    return 360; // upper-right
  return 0;
}

std::pair<std::string, int> Drawer::getWallTexture(int x, int y, const MapData* mapData) {
  auto map = mapData->map();

  auto isWall = [&](int cx, int cy) {
    if (cx < 0 || cy < 0 || cx >= (int)map->Get(cy)->row()->size() || cy >= (int)map->size())
      return false;

    auto tile = map->Get(cy)->row()->Get(cx);
    return tile == Tile_WallHorizontal || tile == Tile_WallVertical;
  };

  bool up = isWall(x, y - 1);
  bool down = isWall(x, y + 1);
  bool left = isWall(x - 1, y);
  bool right = isWall(x + 1, y);

  // Ending walls
  if (up && !down && !left && !right)
    return std::make_pair("assets/wall_end.png", 180);
  if (down && !up && !left && !right)
    return std::make_pair("assets/wall_end.png", 0);
  if (left && !right && !up && !down)
    return std::make_pair("assets/wall_end.png", 90);
  if (right && !left && !up && !down)
    return std::make_pair("assets/wall_end.png", 270);

  // Corner walls
  if (down && right && !up && !left)
    return std::make_pair("assets/corner.png", 0);
  if (down && left && !up && !right)
    return std::make_pair("assets/corner.png", 90);
  if (up && left && !down && !right)
    return std::make_pair("assets/corner.png", 180);
  if (up && right && !down && !left)
    return std::make_pair("assets/corner.png", 270);

  return map->Get(y)->row()->Get(x) == Tile_WallHorizontal ? std::make_pair("assets/wall.png", 90)
                                                           : std::make_pair("assets/wall.png", 0);
}

// TODO: refactor using AnimationManager :)
void Drawer::setTailFrame() {
  static auto lastReadTime = std::chrono::steady_clock::now();

  auto currentTime = std::chrono::steady_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastReadTime).count();

  if (elapsed > 200) {
    if (tailFrame.first == 1) {
      tailFrame = std::make_pair(2, "assets/tail2.png");
    } else if (tailFrame.first == 2) {
      tailFrame = std::make_pair(3, "assets/tail.png");
    } else if (tailFrame.first == 3) {
      tailFrame = std::make_pair(4, "assets/tail3.png");
    } else if (tailFrame.first == 4) {
      tailFrame = std::make_pair(1, "assets/tail.png");
    }
    lastReadTime = currentTime;
  }
}

void Drawer::stopClient() {
  if (!this->clientThread.joinable()) {
	std::cout << "Client is not running" << std::endl;
	return;
  }
    
  this->client->setStopFlag(true);
  this->clientThread.join();
  eventManager->SetCurrentState(StateType::Menu);
}

void Drawer::startClient(const std::string& serverIP, bool isSinglePlayer) {
  if (this->clientThread.joinable()) {
	std::cout << "Client already running" << std::endl;
	return;
  }
  
  const std::string mode = isSinglePlayer ? "Single-player" : "Multiplayer";
  
  this->client->setStopFlag(false);
  this->clientThread = std::thread(&Client::start, this->client, serverIP, isSinglePlayer);
  this->eventManager->SetCurrentState(StateType::Game);
}

void Drawer::MoveUp(t_event* details) {
  (void)details; // Unused, but required by callback signature
  this->client->sendDirection(UP);
}

void Drawer::MoveDown(t_event* details) {
  (void)details;
  this->client->sendDirection(DOWN);
}

void Drawer::MoveLeft(t_event* details) {
  (void)details;
  this->client->sendDirection(LEFT);
}

void Drawer::MoveRight(t_event* details) {
  (void)details;
  this->client->sendDirection(RIGHT);
}

void Drawer::ZoomIn(t_event* details) {
  (void)details;
  tileSize = std::min(SCREEN_HEIGHT / 4, tileSize + 5);
}

void Drawer::ZoomOut(t_event* details) {
  (void)details;
  tileSize = std::max(1, this->tileSize - 5);
}

void Drawer::SwitchLib1(t_event* details) {
  (void)details;
  this->switchLibPath = "../libs/lib1/lib1";
  this->gameRunning = false;
}

void Drawer::SwitchLib2(t_event* details) {
  (void)details;
  this->switchLibPath = "../libs/lib2/lib2";
  this->gameRunning = false;
}

void Drawer::SwitchLib3(t_event* details) {
  (void)details;
  this->switchLibPath = "../libs/lib3/lib3";
  this->gameRunning = false;
}

void Drawer::OnMouseClick(t_event* details) {
  float x = details->mouse.x;
  float y = details->mouse.y;

  if (x >= this->multiplayerButton.x && x <= this->multiplayerButton.x + this->multiplayerButton.width &&
      y >= this->multiplayerButton.y && y <= this->multiplayerButton.y + this->multiplayerButton.height)
    this->startClient(REMOTE_SERVER_IP, false);
  else if (x >= this->singlePlayerButton.x &&
           x <= this->singlePlayerButton.x + this->singlePlayerButton.width &&
           y >= this->singlePlayerButton.y &&
           y <= this->singlePlayerButton.y + this->singlePlayerButton.height)
    this->startClient(LOCAL_SERVER_IP, true);
}
