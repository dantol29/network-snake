#include "../IGraphics.hpp"
#include "Graphics.hpp"
#include <iostream>

static Color toColor(rgb c)
{
    return Color{
        (unsigned char)(c.r * 255.0f),
        (unsigned char)(c.g * 255.0f),
        (unsigned char)(c.b * 255.0f),
        255};
}

Graphics::Graphics(unsigned int height, unsigned int width) : IGraphics(height, width)
{
    InitWindow((int)width, (int)height, "raylib");
    SetTargetFPS(60);
    SetTraceLogLevel(LOG_NONE);

    this->windowWidth = (float)GetScreenWidth();
    this->windowHeight = (float)GetScreenHeight();

    this->font = GetFontDefault();

    if (!FileExists("assets/space.jpeg"))
        throw "Image does not exist 1";

    Image img = LoadImage("assets/space.jpeg");
    if (img.data == nullptr)
        throw "Image does not exist 2";

    this->tex = LoadTextureFromImage(img);
    UnloadImage(img);
    if (this->tex.id == 0)
        throw "Image does not exist 3";
}

Graphics::~Graphics()
{
    std::cout << "Destructor RAYLIB" << std::endl;
    if (tex.id != 0)
        UnloadTexture(tex);

    UnloadFont(font);
    if (!WindowShouldClose())
        CloseWindow();
}

void Graphics::drawSquare(float pixelX, float pixelY, float pixelWidth, float pixelHeight, struct rgb color)
{
    DrawRectangle((int)pixelX, (int)pixelY, (int)pixelWidth, (int)pixelHeight, toColor(color));
}

void Graphics::drawButton(float x, float y, float width, float height, const char *text)
{
    DrawRectangle((int)x, (int)y, (int)width, (int)height, Color{120, 120, 100, 255});

    const int fontSize = 24;
    const float spacing = 4.0f;
    Vector2 size = MeasureTextEx(font, text, (float)fontSize, spacing);

    float tx = x + (width - size.x) * 0.5f;
    float ty = y + (height - size.y) * 0.5f;

    DrawTextEx(font, text, {tx, ty}, (float)fontSize, spacing, WHITE);
}

void Graphics::drawText(float x, float y, int size, const char *text)
{
    DrawTexture(tex, 0, 0, WHITE);

    DrawTextEx(font, text, {x, y}, (float)size, 4.0f, WHITE);
}

void Graphics::beginFrame() 
{
    BeginDrawing();
    ClearBackground(BLACK);
}

void Graphics::endFrame() 
{
    EndDrawing();
}

t_event Graphics::checkEvents()
{
    t_event event;
    event.type = KEY_PRESSED;

    if (WindowShouldClose()) 
        event.type = CLOSED;
    else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        Vector2 mp = GetMousePosition();
        event.type = MOUSE_BUTTON_RELEASED;  // Changed to RELEASED to match keys.cfg (9:0)
        event.mouse.x = (int)mp.x;
        event.mouse.y = (int)mp.y;
        event.mouse.button = 0;  // Left button = 0 (matches keys.cfg)
    }
    // Map raylib key codes to SFML key codes (to match keys.cfg)
    // Raylib: KEY_W=87, KEY_A=65, KEY_S=83, KEY_D=68, KEY_UP=265, KEY_DOWN=264, KEY_LEFT=263, KEY_RIGHT=262
    // SFML: W=22, A=0, S=18, D=3, Up=73, Down=74, Left=71, Right=72
    else if (IsKeyPressed(KEY_W))
        event.keyCode = 22;  // SFML W
    else if (IsKeyPressed(KEY_UP))
        event.keyCode = 73;  // SFML Up
    else if (IsKeyPressed(KEY_S))
        event.keyCode = 18;  // SFML S
    else if (IsKeyPressed(KEY_DOWN))
        event.keyCode = 74;  // SFML Down
    else if (IsKeyPressed(KEY_A))
        event.keyCode = 0;   // SFML A
    else if (IsKeyPressed(KEY_LEFT))
        event.keyCode = 71;  // SFML Left
    else if (IsKeyPressed(KEY_D))
        event.keyCode = 3;   // SFML D
    else if (IsKeyPressed(KEY_RIGHT))
        event.keyCode = 72;  // SFML Right
    else if (IsKeyPressed(KEY_M))
        event.keyCode = 12;  // SFML M (approximate)
    else if (IsKeyPressed(KEY_N))
        event.keyCode = 13;  // SFML N (approximate)
    else if (IsKeyPressed(KEY_ONE))
        event.keyCode = 27;  // SFML Num1 (approximate)
    else if (IsKeyPressed(KEY_TWO))
        event.keyCode = 28;  // SFML Num2 (approximate)
    else if (IsKeyPressed(KEY_THREE))
        event.keyCode = 29;  // SFML Num3 (approximate)
    else
        event.type = EMPTY;

    return event;
}
