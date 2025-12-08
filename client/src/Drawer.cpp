#include "Drawer.hpp"
#include "EventManager.hpp"
#include <fstream>

#define WIDTH 1000
#define HEIGHT 1000
#define INITIAL_SCREEN_SIZE 20
#define SCREEN_LEN 2.0f

Drawer::Drawer(Client* client)
    : client(client), screenSize(INITIAL_SCREEN_SIZE), prevSnakeHeadX(0), prevSnakeHeadY(0),
      switchLibPath("../libs/lib2/lib2"), gameMode(MENU),
      multiplayerButton{400, 300, 200, 60, "Multiplayer", Button::MULTIPLAYER},
      singlePlayerButton{400, 400, 200, 60, "Single-player", Button::SINGLE_PLAYER} {
  tilePx = std::max(1, std::min(WIDTH / screenSize, HEIGHT / screenSize));

  // Initialize EventManager
  eventManager = new EventManager();

  // Register movement callbacks (arrow keys and WASD) - only active in Game
  // state
  eventManager->AddCallback(StateType::Game, "Key_Up", &Drawer::MoveUp, this);
  eventManager->AddCallback(StateType::Game, "Key_Down", &Drawer::MoveDown, this);
  eventManager->AddCallback(StateType::Game, "Key_Left", &Drawer::MoveLeft, this);
  eventManager->AddCallback(StateType::Game, "Key_Right", &Drawer::MoveRight, this);
  eventManager->AddCallback(StateType::Game, "Key_W", &Drawer::MoveUp, this);
  eventManager->AddCallback(StateType::Game, "Key_A", &Drawer::MoveLeft, this);
  eventManager->AddCallback(StateType::Game, "Key_S", &Drawer::MoveDown, this);
  eventManager->AddCallback(StateType::Game, "Key_D", &Drawer::MoveRight, this);

  // Register zoom callbacks
  eventManager->AddCallback(StateType::Game, "Key_M", &Drawer::ZoomIn, this);
  eventManager->AddCallback(StateType::Game, "Key_N", &Drawer::ZoomOut, this);

  // Register mouse callback - only active in Menu state (not during gameplay)
  eventManager->AddCallback(StateType::Menu, "Mouse_Left", &Drawer::OnMouseClick, this);

  // Set initial state to Menu (since we start with the menu)
  eventManager->SetCurrentState(StateType::Menu);

  this->readAssets();

  tailFrame = std::make_pair(1, "assets/tail.png");
}

Drawer::~Drawer() {
  delete eventManager;
  this->closeDynamicLib();

  for (int i = 0; assets[i]; ++i)
    free(assets[i]);
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

        // Handle CLOSED event directly
        if (event.type == CLOSED) {
          gameRunning = false;
        }

        // Pass event to EventManager (only if not EMPTY)
        if (event.type != EMPTY) {
          eventManager->HandleEvent(event);
          eventManager->Update();
        }

        if (this->gameMode == GAME)
          this->drawGameField();
        else
          this->drawMenu();

        this->endFrame(this->window);
      }

      if (this->switchLibPath.empty())
        break;
    }
  } catch (const char* msg) {
    std::cerr << msg << std::endl;
  }

  this->stopClient();
}

void Drawer::readAssets() {
  std::ifstream file("assets.list");
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
  this->window = this->init(HEIGHT, WIDTH, this);
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

void Drawer::drawGameField() {
  if (this->client->getIsDead() || this->client->getStopFlag()) {
    this->stopClient();
    this->gameMode = MENU;
    return;
  }

  const int snakeHeadX = this->client->getSnakeX();
  const int snakeHeadY = this->client->getSnakeY();

  std::mutex& gameFieldMutex = this->client->getGameFieldMutex();
  std::lock_guard<std::mutex> lock(gameFieldMutex);

  const std::vector<std::string>& gameField = this->client->getGameField();
  this->height = this->client->getHeight();
  this->width = this->client->getWidth();
  const int screenCenter = screenSize / 2;
  const int originX = (WIDTH - (tilePx * screenSize)) / 2;
  const int originY = (HEIGHT - (tilePx * screenSize)) / 2;

  setTailFrame();

  for (int sy = 0; sy < screenSize; ++sy) {
    int wy = snakeHeadY + (sy - screenCenter);
    for (int sx = 0; sx < screenSize; ++sx) {
      int wx = snakeHeadX + (sx - screenCenter);
      if (wx < 0 || wx >= width || wy < 0 || wy >= height)
        continue;

      int px = originX + sx * tilePx;
      int py = originY + sy * tilePx;

      this->drawBorder(wx, wy, px, py, tilePx);

      char tile = gameField[wy][wx];
      if (tile == 'F')
        this->drawAsset(this->window, px, py, tilePx, tilePx, 0, "assets/food.png");
      else if (tile == 'B' || tile == 'T')
        this->drawAsset(this->window, px, py, tilePx, tilePx, 0,
                        tile == 'B' ? "assets/body.png" : tailFrame.second.c_str());
      else if (tile == 'H')
        this->drawAsset(this->window, px, py, tilePx, tilePx, 0, "assets/head.png");
      else
        continue;
    }
  }
  this->prevSnakeHeadX = snakeHeadX;
  this->prevSnakeHeadY = snakeHeadY;
}

// TODO: refactor using AnimationManager :)
void Drawer::setTailFrame() {
  static auto lastReadTime = std::chrono::steady_clock::now();

  auto currentTime = std::chrono::steady_clock::now();
  auto elapsed =
      std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastReadTime).count();

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

void Drawer::drawBorder(int x, int y, int px, int py, int tilePx) {
  if (x == 0)
    this->drawAsset(this->window, px - tilePx, py, tilePx, tilePx, 180, "assets/wall.png");

  if (x == this->width - 1)
    this->drawAsset(this->window, px + tilePx, py, tilePx, tilePx, 0, "assets/wall.png");

  if (y == 0)
    this->drawAsset(this->window, px, py - tilePx, tilePx, tilePx, 270, "assets/wall.png");

  if (y == this->height - 1)
    this->drawAsset(this->window, px, py + tilePx, tilePx, tilePx, 90, "assets/wall.png");
}

void Drawer::stopClient() {
  this->client->setStopFlag(true);
  if (this->clientThread.joinable())
    this->clientThread.join();
}

void Drawer::startClient(const std::string& serverIP, bool isSinglePlayer) {
  const std::string mode = isSinglePlayer ? "Single-player" : "Multiplayer";
  std::cout << mode << " mode selected" << '\n';

  if (!this->clientThread.joinable()) {
    this->gameMode = GAME;
    this->client->setIsDead(false);
    this->client->setStopFlag(false);
    this->clientThread = std::thread(&Client::start, this->client, serverIP, isSinglePlayer);
    eventManager->SetCurrentState(StateType::Game);
  }
}

void Drawer::onMouseUp(float x, float y) {
  std::cout << "MouseUp" << std::endl;
  if (x >= this->multiplayerButton.x &&
      x <= this->multiplayerButton.x + this->multiplayerButton.width &&
      y >= this->multiplayerButton.y &&
      y <= this->multiplayerButton.y + this->multiplayerButton.height)
    this->startClient(REMOTE_SERVER_IP, false);
  else if (x >= this->singlePlayerButton.x &&
           x <= this->singlePlayerButton.x + this->singlePlayerButton.width &&
           y >= this->singlePlayerButton.y &&
           y <= this->singlePlayerButton.y + this->singlePlayerButton.height)
    this->startClient(LOCAL_SERVER_IP, true);
}

// EventManager callbacks
void Drawer::MoveUp(MatchedEventDetails* details) {
  (void)details; // Unused, but required by callback signature
  this->client->sendDirection(UP);
}

void Drawer::MoveDown(MatchedEventDetails* details) {
  (void)details;
  this->client->sendDirection(DOWN);
}

void Drawer::MoveLeft(MatchedEventDetails* details) {
  (void)details;
  this->client->sendDirection(LEFT);
}

void Drawer::MoveRight(MatchedEventDetails* details) {
  (void)details;
  this->client->sendDirection(RIGHT);
}

void Drawer::ZoomIn(MatchedEventDetails* details) {
  (void)details;
  this->screenSize = this->screenSize * 1.10 + 0.5;
  this->tilePx = std::max(1, std::min(WIDTH / screenSize, HEIGHT / screenSize));
}

void Drawer::ZoomOut(MatchedEventDetails* details) {
  (void)details;
  this->screenSize = this->screenSize / 1.10;
  this->tilePx = std::max(1, std::min(WIDTH / screenSize, HEIGHT / screenSize));
}

void Drawer::SwitchLib1(MatchedEventDetails* details) {
  (void)details;
  this->switchLibPath = "../libs/lib1/lib1";
  this->gameRunning = false;
}

void Drawer::SwitchLib2(MatchedEventDetails* details) {
  (void)details;
  this->switchLibPath = "../libs/lib2/lib2";
  this->gameRunning = false;
}

void Drawer::SwitchLib3(MatchedEventDetails* details) {
  (void)details;
  this->switchLibPath = "../libs/lib4/lib3";
  this->gameRunning = false;
}

void Drawer::OnMouseClick(MatchedEventDetails* details) {
  std::cout << "MouseClick" << std::endl;
  // Only process mouse clicks when in menu mode
  if (this->gameMode != MENU) {
    return;
  }
  onMouseUp(details->mouse_position_.x, details->mouse_position_.y);
}
