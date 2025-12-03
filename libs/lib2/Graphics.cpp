#include "Graphics.hpp"
#include <iostream>

Graphics::Graphics(unsigned int height, unsigned int width)
    : IGraphics(height, width), gameWindow(sf::VideoMode(sf::Vector2u(width, height)), "SFML")
{
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

    if (this->gameWindow.isOpen())
        this->gameWindow.close();
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

void Graphics::endFrame()
{
    if (this->shouldUpdateScreen && this->gameWindow.isOpen()) 
        this->gameWindow.display();
}

void Graphics::beginFrame()
{
    if (this->shouldUpdateScreen && this->gameWindow.isOpen())
    {
        this->shouldUpdateScreen = false;
        this->gameWindow.clear();
    }
}

t_event Graphics::checkEvents() {
    t_event e;
    e.type = EMPTY;

    while (const std::optional event = this->gameWindow.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            e.type = CLOSED;
            return e;
        }
        else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
            return this->onKeyPress(keyPressed->code);
        else if (const auto *mouseReleased = event->getIf<sf::Event::MouseButtonReleased>())
            return this->onMouseUp(mouseReleased->button, mouseReleased->position);
    }

    return e;
}

t_event Graphics::onMouseUp(const sf::Mouse::Button button, const sf::Vector2i position)
{
    t_event event;
    event.type = MOUSE_BUTTON_PRESSED;

    switch (button)
    {
    case sf::Mouse::Button::Left:
        event.mouse.x = position.x;
        event.mouse.y = position.y;
        break;
    default:
        event.type = EMPTY;
        break;
    }

    return event;
}

t_event Graphics::onKeyPress(sf::Keyboard::Key code)
{
    t_event event;
    event.type = KEY_PRESSED;

    switch (code)
    {
    case sf::Keyboard::Key::W:
    case sf::Keyboard::Key::Up:
        event.keyCode = UP;
        break;
    case sf::Keyboard::Key::S:
    case sf::Keyboard::Key::Down:
        event.keyCode = DOWN;
        break;
    case sf::Keyboard::Key::A:
    case sf::Keyboard::Key::Left:
        event.keyCode = LEFT;
        break;
    case sf::Keyboard::Key::D:
    case sf::Keyboard::Key::Right:
        event.keyCode = RIGHT;
        break;
    case sf::Keyboard::Key::M:
        event.keyCode = M;
        break;
    case sf::Keyboard::Key::N:
        event.keyCode = N;
        break;
    case sf::Keyboard::Key::Num1:
        event.keyCode = KEY_1;
        break;
    case sf::Keyboard::Key::Num2:
        event.keyCode = KEY_2;
        break;
    case sf::Keyboard::Key::Num3:
        event.keyCode = KEY_3;
        break;
    default:
        event.type = EMPTY;
        break;
    }

    return event;
}
