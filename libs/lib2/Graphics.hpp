#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "../IGraphics.hpp"
#include <SFML/Graphics.hpp>

class Graphics: public IGraphics
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
    sf::RenderWindow gameWindow;
    sf::Texture tex;
    sf::Font font;

    t_event onKeyPress(sf::Keyboard::Key key);
    t_event onMouseUp(const sf::Mouse::Button, const sf::Vector2i position);
};

extern "C" IGraphics *init(unsigned int height, unsigned int width)
{
    return new Graphics(height, width);
}

#endif
