#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "../../client/src/Drawer.hpp"
#include <SFML/Graphics.hpp>

class Graphics
{
public:
    Graphics(unsigned int height, unsigned int width, void *gamePointer);
    ~Graphics();

    void loop();
    void display();
    void cleanScreen();
    void closeWindow();
    void drawText(float x, float y, int size, const char *text);
    void drawSquare(float x, float y, float width, float height, struct rgb color);
    void drawButton(float x, float y, float width, float height, const char *text);

private:
    sf::RenderWindow gameWindow;
    sf::Texture tex;
    sf::Font font;
    float windowWidth;
    float windowHeight;

    void keyCallback(sf::Keyboard::Key key);
    void onMouseUp(const sf::Mouse::Button, const sf::Vector2i position);
};

extern "C"
{
    Graphics *init(unsigned int height, unsigned int width, void *game)
    {
        (void)game;
        Graphics *g = new Graphics(height, width, game);
        return g;
    }

    void cleanup(void *g)
    {
        if (g)
            delete static_cast<Graphics *>(g);
    }

    void loop(void *g)
    {
        if (g)
            static_cast<Graphics *>(g)->loop();
    }

    void closeWindow(void *g)
    {
        if (g)
            static_cast<Graphics *>(g)->closeWindow();
    }

    void drawSquare(void *g, float x, float y, float width, float height, struct rgb color)
    {
        if (g)
            static_cast<Graphics *>(g)->drawSquare(x, y, width, height, color);
    }

    void drawButton(void *g, float x, float y, float width, float height, const char *text)
    {
        if (g)
            static_cast<Graphics *>(g)->drawButton(x, y, width, height, text);
    }

    void drawText(void *g, float x, float y, int size, const char *text)
    {
        if (g)
            static_cast<Graphics *>(g)->drawText(x, y, size, text);
    }

    void display(void *g)
    {
        if (g)
            static_cast<Graphics *>(g)->display();
    }

    void cleanScreen(void *g)
    {
        if (g)
            static_cast<Graphics *>(g)->cleanScreen();
    }
}

#endif
