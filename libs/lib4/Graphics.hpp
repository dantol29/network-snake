#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "../IGraphics.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class Graphics : public IGraphics
{
public:
    Graphics(unsigned int height, unsigned int width, void *gamePointer);
    ~Graphics();

    void loop() override;
    void display() override;
    void cleanScreen() override;
    void stopLibrary() override;
    void drawText(float x, float y, int size, const char *text) override;
    void drawSquare(float x, float y, float width, float height, struct rgb color) override;
    void drawButton(float x, float y, float width, float height, const char *text) override;

private:
    SDL_Window* gameWindow = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;

    void keyCallback(const SDL_KeyboardEvent& keyEvent);
    void onMouseUp(const SDL_MouseButtonEvent& buttonEvent);
};

extern "C" IGraphics *init(unsigned int height, unsigned int width, void *game)
{
    return new Graphics(height, width, game);
}

#endif
