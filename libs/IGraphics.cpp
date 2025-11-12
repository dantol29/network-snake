#include "IGraphics.hpp"

IGraphics::IGraphics(unsigned int height, unsigned int width, void *gamePointer) {
    drawer = static_cast<Drawer *>(gamePointer);
    windowWidth = static_cast<float>(width);
    windowHeight = static_cast<float>(height);
    running = true;
};

void cleanup(void *g)
{
    if (g)
        delete static_cast<IGraphics *>(g);
}

void loop(void *g)
{
    if (g)
        static_cast<IGraphics *>(g)->loop();
}

void stopLibrary(void *g)
{
    if (g)
        static_cast<IGraphics *>(g)->stopLibrary();
}

void drawSquare(void *g, float x, float y, float width, float height, struct rgb color)
{
    if (g)
        static_cast<IGraphics *>(g)->drawSquare(x, y, width, height, color);
}

void drawButton(void *g, float x, float y, float width, float height, const char *text)
{
    if (g)
        static_cast<IGraphics *>(g)->drawButton(x, y, width, height, text);
}

void drawText(void *g, float x, float y, int size, const char *text)
{
    if (g)
        static_cast<IGraphics *>(g)->drawText(x, y, size, text);
}

void display(void *g)
{
    if (g)
        static_cast<IGraphics *>(g)->display();
}

void cleanScreen(void *g)
{
    if (g)
        static_cast<IGraphics *>(g)->cleanScreen();
}
