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

Drawer *drawer = nullptr;

Graphics::Graphics(unsigned int height, unsigned int width, void *gamePointer) : isClosed(false)
{
    InitWindow((int)width, (int)height, "raylib");
    SetTargetFPS(60);
    SetTraceLogLevel(LOG_NONE);

    windowWidth = (float)GetScreenWidth();
    windowHeight = (float)GetScreenHeight();

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

    drawer = static_cast<Drawer *>(gamePointer);
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

void Graphics::closeWindow()
{
    CloseWindow();
    this->isClosed = true;
}

void Graphics::loop()
{
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        drawer->onEachFrame(true);
        this->checkEvents();

        if (!this->isClosed)
            EndDrawing();
    }

    std::cout << "Exit loop 1" << std::endl;
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

void Graphics::display()
{
}

void Graphics::cleanScreen()
{
}

void Graphics::checkEvents()
{
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        Vector2 mp = GetMousePosition();
        drawer->onMouseUp((int)mp.x, (int)mp.y);
    }

    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
        drawer->keyCallback(UP, 1);
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
        drawer->keyCallback(DOWN, 1);
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))
        drawer->keyCallback(LEFT, 1);
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT))
        drawer->keyCallback(RIGHT, 1);
    if (IsKeyPressed(KEY_M))
        drawer->keyCallback(M, 1);
    if (IsKeyPressed(KEY_N))
        drawer->keyCallback(N, 1);
    if (IsKeyPressed(KEY_ONE))
        drawer->keyCallback(KEY_1, 1);
    if (IsKeyPressed(KEY_TWO))
        drawer->keyCallback(KEY_2, 1);
    if (IsKeyPressed(KEY_THREE))
        drawer->keyCallback(KEY_3, 1);
}
