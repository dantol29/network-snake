#include "IGraphics.hpp"

IGraphics::IGraphics(unsigned int height, unsigned int width)
{
    windowWidth = static_cast<float>(width);
    windowHeight = static_cast<float>(height);
    shouldUpdateScreen = false;
};

void IGraphics::setShouldUpdateScreen(bool value) 
{
    shouldUpdateScreen = value;
}

void cleanup(void *g)
{
    if (g)
        delete static_cast<IGraphics *>(g);
}

t_event checkEvents(void *g)
{
    return static_cast<IGraphics *>(g)->checkEvents();
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

void beginFrame(void *g)
{
    if (g)
        static_cast<IGraphics *>(g)->beginFrame();
}

void endFrame(void *g)
{
    if (g)
        static_cast<IGraphics *>(g)->endFrame();
}

void setShouldUpdateScreen(void *g, bool value)
{
    if (g)
        static_cast<IGraphics *>(g)->setShouldUpdateScreen(value);
}