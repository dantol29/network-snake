#pragma once

#include "BaseState.hpp"
#include "EventManager.hpp"
#include <SFML/Graphics.hpp>

class StateMainMenu : public BaseState {
public:
    StateMainMenu(StateManager* l_stateManager);
    ~StateMainMenu();

    void OnCreate() override;
    void OnDestroy() override;
    void Activate() override;
    void Deactivate() override;
    void Update(const sf::Time& l_time) override;
    void Draw() override;

    void MouseClick(EventDetails* l_details);

private:
    sf::Font m_font;
    sf::Text m_text;
    sf::Vector2f m_buttonSize;
    sf::Vector2f m_buttonPos;
    unsigned int m_buttonPadding;
    sf::RectangleShape m_rects[4];
    sf::Text m_labels[4];
};

// SFML types used in this file:
// - sf::Font: Font object used for text rendering (m_font)
// - sf::Text: Text object used for displaying menu title and button labels (m_text, m_labels)
// - sf::Vector2f: 2D vector with float components, used for button size and position
// - sf::RectangleShape: Drawable rectangle shape, used for button backgrounds (m_rects)
// - sf::Time: Time duration type, used in Update() method parameter
// - sf::RenderWindow: Window object used for rendering graphics, accessed through shared context






