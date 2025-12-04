#include "IGraphics.hpp"

IGraphics::IGraphics(unsigned int height, unsigned int width)
{
    windowWidth = static_cast<float>(width);
    windowHeight = static_cast<float>(height);
};

void cleanup(void *g)
{
    if (g)
        delete static_cast<IGraphics *>(g);
}

void loadAssets(void *g, const char **paths)
{
    if (g)
        static_cast<IGraphics *>(g)->loadAssets(paths);
}

t_event checkEvents(void *g)
{
    return static_cast<IGraphics *>(g)->checkEvents();
}

void drawAsset(void *g, float x, float y, float width, float height, const char *assetPath)
{
    if (g)
        static_cast<IGraphics *>(g)->drawAsset(x, y, width, height, assetPath);
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