#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "../IGraphics.hpp"
#include <raylib.h>

class Graphics : public IGraphics
{
public:
    Graphics(unsigned int height, unsigned int width);
    Graphics(const Graphics& obj) = delete;
    Graphics& operator=(const Graphics& obj) = delete;
    ~Graphics();

    t_event checkEvents() override;
    void beginFrame() override;
    void endFrame() override;
    void drawText(float x, float y, int size, const char *text) override;
    void drawSquare(float x, float y, float width, float height, struct rgb color) override;
    void drawButton(float x, float y, float width, float height, const char *text) override;

private:
    Font font{};
    Texture2D tex{};
};

extern "C" IGraphics *init(unsigned int height, unsigned int width)
{
    return new Graphics(height, width);
}


#endif
