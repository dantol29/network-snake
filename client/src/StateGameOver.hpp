#pragma once

#include "BaseState.hpp"
#include "EventManager.hpp"
#include <SFML/Graphics.hpp>

class StateGameOver : public BaseState {
public:
    StateGameOver(StateManager* l_stateManager);
    ~StateGameOver();

    void OnCreate() override;
    void OnDestroy() override;
    void Activate() override;
    void Deactivate() override;
    void Update(const sf::Time& l_time) override;
    void Draw() override;

    void ReturnToMenu(EventDetails* l_details);

private:
    sf::Font m_font;
    sf::Text m_text;
    sf::Text m_instructionText;
    sf::RectangleShape m_rect;
};

