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
    event.type = KEY;

    if (WindowShouldClose()) 
        event.type = EXIT;
    else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        Vector2 mp = GetMousePosition();
        event.type = MOUSE;
        event.mouse.x = (int)mp.x;
        event.mouse.y = (int)mp.y;
    }
    else if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
        event.keyCode = UP;
    else if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
        event.keyCode = DOWN;
    else if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))
        event.keyCode = LEFT;
    else if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT))
        event.keyCode = RIGHT;
    else if (IsKeyPressed(KEY_M))
        event.keyCode = M;
    else if (IsKeyPressed(KEY_N))
        event.keyCode = N;
    else if (IsKeyPressed(KEY_ONE))
        event.keyCode = KEY_1;
    else if (IsKeyPressed(KEY_TWO))
        event.keyCode = KEY_2;
    else if (IsKeyPressed(KEY_THREE))
        event.keyCode = KEY_3;
    else
        event.type = EMPTY;

    return event;
}
