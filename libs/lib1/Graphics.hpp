#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "../../client/src/Drawer.hpp"
#include <GLFW/glfw3.h>

class Graphics

{
private:
    GLFWwindow *window = nullptr;
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

public:
    Graphics(int width, int height, void *game);
    ~Graphics();

    void loop();
    void drawSquare(GLfloat x, GLfloat y, GLfloat width, GLfloat height, struct rgb color) const;
};

extern "C"
{
    Graphics *init(int height, int width, void *game)
    {
        Graphics *graphics = new Graphics(height, width, game);
        return graphics;
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

    void drawSquare(void *g, float x, float y, float width, float height, struct rgb color)
    {
        if (g)
            static_cast<Graphics *>(g)->drawSquare(x, y, width, height, color);
    }
};

#endif
