#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "../../client/src/Drawer.hpp"
#include <raylib.h>

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
    Font font{};
    Texture2D tex{};

    void checkEvents();
};

extern "C" IGraphics *init(unsigned int height, unsigned int width, void *game)
{
    return new Graphics(height, width, game);
}


#endif
