#pragma once

#include "BaseState.hpp"
#include "EventManager.hpp"
#include <SFML/Graphics.hpp>

class StatePaused : public BaseState {
public:
    StatePaused(StateManager* l_stateManager);
    ~StatePaused();

    void OnCreate() override;
    void OnDestroy() override;
    void Activate() override;
    void Deactivate() override;
    void Update(const sf::Time& l_time) override;
    void Draw() override;

    void Unpause(EventDetails* l_details);

private:
    sf::Font m_font;
    sf::Text m_text;
    sf::RectangleShape m_rect;
};

// SFML types used in this file:
// - sf::Font: Font object used for text rendering (m_font)
// - sf::Text: Text object used for displaying "PAUSED" message (m_text)
// - sf::RectangleShape: Drawable rectangle shape, used for semi-transparent backdrop (m_rect)
// - sf::Time: Time duration type, used in Update() method parameter
// - sf::RenderWindow: Window object used for rendering graphics, accessed through shared context

