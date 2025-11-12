#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "../IGraphics.hpp"
#include <SFML/Graphics.hpp>

class Graphics: public IGraphics
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
    sf::RenderWindow gameWindow;
    sf::Texture tex;
    sf::Font font;

    void keyCallback(sf::Keyboard::Key key);
    void onMouseUp(const sf::Mouse::Button, const sf::Vector2i position);
};

extern "C" IGraphics *init(unsigned int height, unsigned int width, void *game)
{
    return new Graphics(height, width, game);
}

#endif
