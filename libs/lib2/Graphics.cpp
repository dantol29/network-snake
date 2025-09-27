#include "Graphics.hpp"
#include <iostream>

Drawer *drawer = nullptr;

Graphics::Graphics(unsigned int height, unsigned int width, void *gamePointer)
    : gameWindow(sf::VideoMode(sf::Vector2u(width, height)), "SFML")
{
    drawer = static_cast<Drawer *>(gamePointer);
    this->windowWidth = static_cast<float>(this->gameWindow.getSize().x);
    this->windowHeight = static_cast<float>(this->gameWindow.getSize().y);
}

Graphics::~Graphics() {}

void Graphics::closeWindow()
{
    this->gameWindow.close();
}

void Graphics::loop()
{
    while (this->gameWindow.isOpen())
    {
        while (const std::optional event = this->gameWindow.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                this->gameWindow.close();
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
                this->keyCallback(keyPressed->code);
        }

        drawer->drawGameField();
    }

    std::cout << "Exit loop 2" << std::endl;
}

void Graphics::drawSquare(float x, float y, float width, float height, struct rgb color)
{
    // Normalized coordinates (-1.0 to 1.0) to pixel coordinates
    float pixelX = (x + 1.0f) * this->windowWidth / 2.0f;
    float pixelY = (1.0f - y) * this->windowHeight / 2.0f;
    float pixelWidth = width * this->windowWidth / 2.0f;
    float pixelHeight = height * this->windowHeight / 2.0f;

    sf::RectangleShape rectangle(sf::Vector2f(pixelWidth, pixelHeight));
    rectangle.setPosition(sf::Vector2f(pixelX, pixelY));
    rectangle.setFillColor(sf::Color(
        static_cast<std::uint8_t>(color.r * 255),
        static_cast<std::uint8_t>(color.g * 255),
        static_cast<std::uint8_t>(color.b * 255)));

    this->gameWindow.draw(rectangle);
}

void Graphics::display()
{
    this->gameWindow.display();
}

void Graphics::cleanScreen()
{
    this->gameWindow.clear();
}

void Graphics::keyCallback(sf::Keyboard::Key code)
{
    switch (code)
    {
    case sf::Keyboard::Key::W:
    case sf::Keyboard::Key::Up:
        drawer->keyCallback(UP, 1);
        break;
    case sf::Keyboard::Key::S:
    case sf::Keyboard::Key::Down:
        drawer->keyCallback(DOWN, 1);
        break;
    case sf::Keyboard::Key::A:
    case sf::Keyboard::Key::Left:
        drawer->keyCallback(LEFT, 1);
        break;
    case sf::Keyboard::Key::D:
    case sf::Keyboard::Key::Right:
        drawer->keyCallback(RIGHT, 1);
        break;
    case sf::Keyboard::Key::M:
        drawer->keyCallback(M, 1);
        break;
    case sf::Keyboard::Key::N:
        drawer->keyCallback(N, 1);
        break;
    case sf::Keyboard::Key::Num1:
        drawer->keyCallback(KEY_1, 1);
        break;
    case sf::Keyboard::Key::Num2:
        drawer->keyCallback(KEY_2, 1);
        break;
    default:
        break;
    }
}
