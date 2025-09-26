#include "Graphics.hpp"
#include <unistd.h>
#include <stdlib.h>

#define GL_COLOR_BUFFER_BIT 0x00004000

Drawer *drawer = nullptr;

Graphics::Graphics(int width, int height, void *gamePointer)
{
    if (!glfwInit())
        return; // TODO: handle error

    this->window = (GLFWwindow *)glfwCreateWindow(width, height, "GLFW", NULL, NULL);
    if (!this->window)
        return;

    glfwMakeContextCurrent(this->window); // enable openGL
    glfwSwapInterval(1);                  // wait for one monitor refresh per buffer swap
    glfwSetKeyCallback(this->window, Graphics::keyCallback);
    drawer = static_cast<Drawer *>(gamePointer);
}

Graphics::~Graphics()
{
    glfwDestroyWindow(this->window);
    glfwTerminate();
}

void Graphics::closeWindow()
{
    glfwSetWindowShouldClose(this->window, GLFW_TRUE);
}

void Graphics::loop()
{
    while (!glfwWindowShouldClose(this->window))
    {
        drawer->drawGameField();
        glfwPollEvents();
    }

    std::cout << "Exit loop 1" << std::endl;
}

void Graphics::drawSquare(GLfloat x, GLfloat y, GLfloat width, GLfloat height, struct rgb color) const
{
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);                  // bottom-left
    glVertex2f(x + width, y);          // bottom-right
    glVertex2f(x + width, y + height); // top-right
    glVertex2f(x, y + height);         // top-left
    glEnd();
}

void Graphics::display()
{
    if (!this->window)
        return;

    glfwSwapBuffers(this->window); // Render window
}

void Graphics::cleanScreen()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black
    glClear(GL_COLOR_BUFFER_BIT);
}

void Graphics::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    // TODO: convert key and actions to standard
    drawer->keyCallback(key, action);
}
