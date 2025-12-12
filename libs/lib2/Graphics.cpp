#include "Graphics.hpp"
#include <iostream>

Graphics::Graphics(unsigned int height, unsigned int width)
    : IGraphics(height, width), gameWindow(sf::VideoMode(sf::Vector2u(width, height)), "SFML") {
  this->windowWidth = static_cast<float>(this->gameWindow.getSize().x);
  this->windowHeight = static_cast<float>(this->gameWindow.getSize().y);

  if (!font.openFromFile("assets/Montserrat-Bold.ttf"))
    throw "Font does not exist";
}

Graphics::~Graphics() {
  std::cout << "Destructor SFML" << std::endl;

  if (this->gameWindow.isOpen())
    this->gameWindow.close();
}

void Graphics::loadAssets(const char** paths) {
  if (!paths)
    return;

  for (int i = 0; paths[i]; i++) {
    sf::Texture texture;
    if (!texture.loadFromFile(paths[i]))
      throw "Failed to load assets";

    this->assets.insert({std::string(paths[i]), texture});
  }
}

void Graphics::drawAsset(float pixelX, float pixelY, float pixelWidth, float pixelHeight,
                         int degrees, const char* assetPath) {
  try {
    sf::Texture asset = assets.at(std::string(assetPath));

    sf::Sprite sprite(asset);

    sf::Vector2u size = asset.getSize();
    sprite.setOrigin(sf::Vector2f(size.x / 2.f, size.y / 2.f));

    float centerX = pixelX + pixelWidth / 2.f;
    float centerY = pixelY + pixelHeight / 2.f;
    sprite.setPosition(sf::Vector2f(centerX, centerY));

    float scaleX = pixelWidth / size.x;
    float scaleY = pixelHeight / size.y;
    sprite.setScale(sf::Vector2f(scaleX, scaleY));

    sprite.setRotation(sf::degrees(degrees));

    gameWindow.draw(sprite);
  } catch (const std::out_of_range& e) {
    std::cout << "Key not found!\n" << std::endl;
  }
}

void Graphics::drawButton(float x, float y, float width, float height, const char* text) {
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

  label.setPosition(
      sf::Vector2f{boxPos.x + (boxSize.x - textBounds.size.x) * 0.5f - textBounds.position.x,
                   boxPos.y + (boxSize.y - textBounds.size.y) * 0.5f - textBounds.position.y});

  this->gameWindow.draw(box);
  this->gameWindow.draw(label);
}

void Graphics::drawText(float x, float y, int size, const char* text) {
  sf::Text label(font);
  label.setString(text);
  label.setCharacterSize(size);
  label.setPosition(sf::Vector2f{x, y});

  this->gameWindow.draw(label);
}

void Graphics::endFrame() {
  if (this->gameWindow.isOpen())
    this->gameWindow.display();
}

void Graphics::beginFrame() {
  if (this->gameWindow.isOpen())
    this->gameWindow.clear();
}

t_event Graphics::checkEvents() {
  t_event e;
  e.type = EMPTY;

  while (const std::optional event = this->gameWindow.pollEvent()) {
    if (event->is<sf::Event::Closed>()) {
      e.type = CLOSED;
      return e;
    } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
      return this->onKeyPress(keyPressed->code);
    else if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>())
      return this->onMouseUp(mouseReleased->button, mouseReleased->position);
  }

  return e;
}

t_event Graphics::onMouseUp(const sf::Mouse::Button button, const sf::Vector2i position) {
  t_event event;
  event.type = MOUSE_BUTTON_RELEASED;

  switch (button) {
  case sf::Mouse::Button::Left:
    event.mouse.x = position.x;
    event.mouse.y = position.y;
    event.mouse.button = 0;
    break;
  default:
    event.type = EMPTY;
    break;
  }

  return event;
}

t_event Graphics::onKeyPress(sf::Keyboard::Key code) {
  t_event event;
  event.type = KEY_PRESSED;
  event.keyCode = static_cast<int>(code);

  return event;
}
