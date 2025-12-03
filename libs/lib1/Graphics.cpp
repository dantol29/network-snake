#include "../IGraphics.hpp"
#include "Graphics.hpp"
#include <iostream>

Graphics::Graphics(unsigned int height, unsigned int width) : IGraphics(height, width)
{
    InitWindow((int)width, (int)height, "raylib");
    SetTargetFPS(60);
    SetTraceLogLevel(LOG_NONE);

    this->windowWidth = (float)GetScreenWidth();
    this->windowHeight = (float)GetScreenHeight();

    this->font = GetFontDefault();
}

Graphics::~Graphics()
{
    std::cout << "Destructor RAYLIB" << std::endl;
    for (auto asset : assets)
        UnloadTexture(asset.second);

    if (!WindowShouldClose())
        CloseWindow();
}

void Graphics::loadAssets(const char **paths)
{
    if (!paths)
        return;

    for (int i = 0; paths[i]; ++i)
    {
        if (!FileExists(paths[i]))
        {
            std::cout << "Image does not exist: " << paths[i] << std::endl;
            continue;
        }

        Image img = LoadImage(paths[i]);
        if (img.data == nullptr)
        {
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

void Graphics::drawAsset(float pixelX, float pixelY, float pixelWidth, float pixelHeight, const char *assetPath)
{
    try
    {
        Texture2D tex = assets.at(assetPath);

        Rectangle src = {0, 0, (float)tex.width, (float)tex.height};
        Rectangle dest = {pixelX, pixelY, pixelWidth, pixelHeight};
        Vector2 origin = {0, 0};

        DrawTexturePro(tex, src, dest, origin, 0.0f, WHITE);
    }
    catch (const std::out_of_range &e)
    {
        std::cout << "Key not found!\n"
                  << std::endl;
    }
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
        event.a = (int)mp.x;
        event.b = (int)mp.y;
    }
    else if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
        event.a = UP;
    else if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
        event.a = DOWN;
    else if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))
        event.a = LEFT;
    else if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT))
        event.a = RIGHT;
    else if (IsKeyPressed(KEY_M))
        event.a = M;
    else if (IsKeyPressed(KEY_N))
        event.a = N;
    else if (IsKeyPressed(KEY_ONE))
        event.a = KEY_1;
    else if (IsKeyPressed(KEY_TWO))
        event.a = KEY_2;
    else if (IsKeyPressed(KEY_THREE))
        event.a = KEY_3;
    else
        event.type = EMPTY;

    return event;
}
