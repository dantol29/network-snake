#include "Graphics.hpp"
#include "../IGraphics.hpp"
#include <iostream>

Graphics::Graphics(unsigned int height, unsigned int width) : IGraphics(height, width) {
  InitWindow((int)width, (int)height, "raylib");
  SetTargetFPS(60);
  SetTraceLogLevel(LOG_NONE);

  this->windowWidth = (float)GetScreenWidth();
  this->windowHeight = (float)GetScreenHeight();

  this->font = GetFontDefault();
}

Graphics::~Graphics() {
  std::cout << "Destructor RAYLIB" << std::endl;
  for (auto asset : assets)
    UnloadTexture(asset.second);

  if (!WindowShouldClose())
    CloseWindow();
}

void Graphics::loadAssets(const char** paths) {
  if (!paths)
    return;

  for (int i = 0; paths[i]; ++i) {
    if (!FileExists(paths[i])) {
      std::cout << "Image does not exist: " << paths[i] << std::endl;
      continue;
    }

    Image img = LoadImage(paths[i]);
    if (img.data == nullptr) {
      std::cout << "Failed to load image: " << paths[i] << std::endl;
      continue;
    }

    Texture2D tex = LoadTextureFromImage(img);
    if (tex.id == 0)
      throw "Failed to load texture";
    UnloadImage(img);

    assets.insert({std::string(paths[i]), tex});
  }
}

void Graphics::drawAsset(float pixelX, float pixelY, float pixelWidth, float pixelHeight,
                         int degrees, const char* assetPath) {
  try {
    Texture2D tex = assets.at(assetPath);

    Rectangle src = {0, 0, (float)tex.width, (float)tex.height};
    Rectangle dest = {pixelX, pixelY, pixelWidth, pixelHeight};

    // center of the destination rectangle
    Vector2 origin = {pixelWidth / 2.f, pixelHeight / 2.f};

    DrawTexturePro(tex, src, dest, origin, (float)degrees, WHITE);
  } catch (const std::out_of_range& e) {
    std::cout << "Key not found!\n" << std::endl;
  }
}

void Graphics::drawButton(float x, float y, float width, float height, const char* text) {
  DrawRectangle((int)x, (int)y, (int)width, (int)height, Color{120, 120, 100, 255});

  const int fontSize = 24;
  const float spacing = 4.0f;
  Vector2 size = MeasureTextEx(font, text, (float)fontSize, spacing);

  float tx = x + (width - size.x) * 0.5f;
  float ty = y + (height - size.y) * 0.5f;

  DrawTextEx(font, text, {tx, ty}, (float)fontSize, spacing, WHITE);
}

void Graphics::drawText(float x, float y, int size, const char* text) {
  DrawTextEx(font, text, {x, y}, (float)size, 4.0f, WHITE);
}

void Graphics::beginFrame() {
  BeginDrawing();
  ClearBackground(BLACK);
}

void Graphics::endFrame() { EndDrawing(); }

t_event Graphics::checkEvents() {
  t_event event;
  event.type = KEY_PRESSED;

  if (WindowShouldClose())
    event.type = CLOSED;
  else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
    Vector2 mp = GetMousePosition();
    event.type = MOUSE_BUTTON_RELEASED;
    event.mouse.x = (int)mp.x;
    event.mouse.y = (int)mp.y;
    event.mouse.button = 0; // Left button = 0
  } else if (IsKeyPressed(KEY_W))
    event.keyCode = 22; // SFML W
  else if (IsKeyPressed(KEY_UP))
    event.keyCode = 73; // SFML Up
  else if (IsKeyPressed(KEY_S))
    event.keyCode = 18; // SFML S
  else if (IsKeyPressed(KEY_DOWN))
    event.keyCode = 74; // SFML Down
  else if (IsKeyPressed(KEY_A))
    event.keyCode = 0; // SFML A
  else if (IsKeyPressed(KEY_LEFT))
    event.keyCode = 71; // SFML Left
  else if (IsKeyPressed(KEY_D))
    event.keyCode = 3; // SFML D
  else if (IsKeyPressed(KEY_RIGHT))
    event.keyCode = 72; // SFML Right
  else if (IsKeyPressed(KEY_M))
    event.keyCode = 12; // SFML M (approximate)
  else if (IsKeyPressed(KEY_N))
    event.keyCode = 13; // SFML N (approximate)
  else if (IsKeyPressed(KEY_ONE))
    event.keyCode = 27; // SFML Num1 (approximate)
  else if (IsKeyPressed(KEY_TWO))
    event.keyCode = 28; // SFML Num2 (approximate)
  else if (IsKeyPressed(KEY_THREE))
    event.keyCode = 29; // SFML Num3 (approximate)
  else
    event.type = EMPTY;

  return event;
}
