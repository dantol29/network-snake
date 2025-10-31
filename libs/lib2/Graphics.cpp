#include "Graphics.hpp"
#include <iostream>

Drawer *drawer = nullptr;

Graphics::Graphics(unsigned int height, unsigned int width, void *gamePointer)
    : gameWindow(sf::VideoMode(sf::Vector2u(width, height)), "SFML")
{
    drawer = static_cast<Drawer *>(gamePointer);
    this->windowWidth = static_cast<float>(this->gameWindow.getSize().x);
    this->windowHeight = static_cast<float>(this->gameWindow.getSize().y);

    if (!font.openFromFile("assets/Montserrat-Bold.ttf"))
        std::cout << "Could not open" << std::endl;

    if (!tex.loadFromFile("assets/space.jpeg"))
        throw "Image does not exist";
}

Graphics::~Graphics()
{
    std::cout << "Destructor SFML" << std::endl;
}

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
            else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
                this->keyCallback(keyPressed->code);
            else if (const auto *mouseReleased = event->getIf<sf::Event::MouseButtonReleased>())
                this->onMouseUp(mouseReleased->button, mouseReleased->position);
        }

        drawer->onEachFrame(false);
    }
}

void Graphics::drawSquare(float pixelX, float pixelY, float pixelWidth, float pixelHeight, struct rgb color)
{
    sf::RectangleShape rectangle(sf::Vector2f(pixelWidth, pixelHeight));
    rectangle.setPosition(sf::Vector2f(pixelX, pixelY));
    rectangle.setFillColor(sf::Color(
        static_cast<std::uint8_t>(color.r * 255),
        static_cast<std::uint8_t>(color.g * 255),
        static_cast<std::uint8_t>(color.b * 255)));

    this->gameWindow.draw(rectangle);
}

void Graphics::drawButton(float x, float y, float width, float height, const char *text)
{
    sf::RectangleShape box;
    box.setSize({width, height});
    box.setPosition({x, y});
    box.setFillColor(sf::Color(120, 120, 100));

    sf::Text label(font);
    label.setString(text);
    label.setCharacterSize(24);

    const auto textBounds = label.getLocalBounds();
    const auto boxPos = box.getPosition();
    const auto boxSize = box.getSize();

    label.setPosition(sf::Vector2f{
        boxPos.x + (boxSize.x - textBounds.size.x) * 0.5f - textBounds.position.x,
        boxPos.y + (boxSize.y - textBounds.size.y) * 0.5f - textBounds.position.y});

    this->gameWindow.draw(box);
    this->gameWindow.draw(label);
}

void Graphics::drawText(float x, float y, int size, const char *text)
{
    sf::Text label(font);
    label.setString(text);
    label.setCharacterSize(size);
    label.setPosition(sf::Vector2f{x, y});

    sf::Sprite sprite(tex);

    this->gameWindow.draw(sprite);
    this->gameWindow.draw(label);
}

void Graphics::display()
{
    this->gameWindow.display();
}

void Graphics::cleanScreen()
{
    this->gameWindow.clear();
}

void Graphics::onMouseUp(const sf::Mouse::Button button, const sf::Vector2i position)
{
    switch (button)
    {
    case sf::Mouse::Button::Left:
        drawer->onMouseUp(position.x, position.y);
    default:
        break;
    }
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
    case sf::Keyboard::Key::Num3:
        drawer->keyCallback(KEY_3, 1);
        break;
    default:
        break;
    }
}
