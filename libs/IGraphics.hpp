#ifndef IGRAPHICS_HPP
#define IGRAPHICS_HPP

#include "../client/src/Drawer.hpp"

class IGraphics
{
public:
    IGraphics(unsigned int height, unsigned int width, void *gamePointer);
    virtual ~IGraphics() = default;

    virtual void loop() = 0;
    virtual void display() = 0;
    virtual void cleanScreen() = 0;
    virtual void stopLibrary() = 0;
    virtual void drawText(float x, float y, int size, const char *text) = 0;
    virtual void drawSquare(float x, float y, float width, float height, struct rgb color) = 0;
    virtual void drawButton(float x, float y, float width, float height, const char *text) = 0;

protected:
    Drawer *drawer;
    float windowWidth;
    float windowHeight;
    bool running;
};

extern "C" {
    IGraphics *init(unsigned int height, unsigned int width, void *game);

    void cleanup(void *g);
    void loop(void *g);
    void stopLibrary(void *g);
    void drawSquare(void *g, float x, float y, float width, float height, struct rgb color);
    void drawButton(void *g, float x, float y, float width, float height, const char *text);
    void drawText(void *g, float x, float y, int size, const char *text);
    void display(void *g);
    void cleanScreen(void *g);
}

#endif