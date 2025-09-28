#include "Graphics.hpp"
#include <unistd.h>
#include <stdlib.h>

#define GL_COLOR_BUFFER_BIT 0x00004000

Drawer *drawer = nullptr;

Graphics::Graphics(int width, int height, void *gamePointer)
{
    if (!glfwInit())
        throw "Failed to init GLFW";

    this->window = (GLFWwindow *)glfwCreateWindow(width, height, "GLFW", NULL, NULL);
    if (!this->window)
        throw "Failed to create window in GLFW";

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
    (void)window;
    (void)scancode;
    (void)mods;

    if (action == 1)
    {
        switch (key)
        {
        case GLFW_KEY_W:
        case GLFW_KEY_UP:
            drawer->keyCallback(UP, action);
            break;
        case GLFW_KEY_S:
        case GLFW_KEY_DOWN:
            drawer->keyCallback(DOWN, action);
            break;
        case GLFW_KEY_A:
        case GLFW_KEY_LEFT:
            drawer->keyCallback(LEFT, action);
            break;
        case GLFW_KEY_D:
        case GLFW_KEY_RIGHT:
            drawer->keyCallback(RIGHT, action);
            break;
        case GLFW_KEY_1:
            drawer->keyCallback(KEY_1, action);
            break;
        case GLFW_KEY_2:
            drawer->keyCallback(KEY_2, action);
            break;
        case GLFW_KEY_M:
            drawer->keyCallback(M, action);
            break;
        case GLFW_KEY_N:
            drawer->keyCallback(N, action);
            break;
        }
    }
}
