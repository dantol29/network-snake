#pragma once

#include "BaseState.hpp"
#include "EventManager.hpp"
#include <SFML/Graphics.hpp>
// #include <chrono> // COMMENTED OUT: Caused AddressSanitizer heap buffer overflow

class StateIntro : public BaseState {
public:
    StateIntro(StateManager* l_stateManager);
    ~StateIntro();

    void OnCreate() override;
    void OnDestroy() override;
    void Activate() override;
    void Deactivate() override;
    void Update(const sf::Time& l_time) override;
    void Draw() override;

    void Continue(EventDetails* l_details);

private:
    sf::Texture m_introTexture;
    sf::Sprite m_introSprite;
    sf::Font m_font;
    sf::Text m_text;
    float m_timePassed;
    // std::chrono::steady_clock::time_point m_startTime; // COMMENTED OUT: For timing verification - caused AddressSanitizer heap buffer overflow
};

// SFML types used in this file:
// - sf::Texture: Image texture, used for m_introTexture
// - sf::Sprite: Drawable sprite, used for m_introSprite
// - sf::Font: Font object used for text rendering (m_font)
// - sf::Text: Text object used for displaying messages (m_text)
// - sf::Time: Time duration type, used in Update() method parameter
// - sf::Vector2u: 2D vector with unsigned integer components, used for window size
// - sf::FloatRect: Rectangle with float components, used for text bounds calculation
// - sf::RenderWindow: Window object used for rendering graphics, accessed through shared context


