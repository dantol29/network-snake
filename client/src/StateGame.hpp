#pragma once

#include "BaseState.hpp"
#include "EventManager.hpp"
#include "World.hpp"
#include "Snake.hpp"
#include "TextBox.hpp"
#include <SFML/Graphics.hpp>

class StateGame : public BaseState {
public:
    StateGame(StateManager* l_stateManager);
    ~StateGame();

    void OnCreate() override;
    void OnDestroy() override;
    void Activate() override;
    void Deactivate() override;
    void Update(const sf::Time& l_time) override;
    void Draw() override;

    // Input callbacks
    void MoveUp(EventDetails* l_details);
    void MoveDown(EventDetails* l_details);
    void MoveLeft(EventDetails* l_details);
    void MoveRight(EventDetails* l_details);
    void Pause(EventDetails* l_details);
    void MainMenu(EventDetails* l_details);

private:
    World m_world;
    Snake m_snake;
    Textbox m_textbox;
    bool m_isDying; // Whether snake is in death animation
    float m_deathAnimationTime; // Time remaining for death animation
};

// SFML types used in this file:
// - sf::Time: Time duration type, used in Update() method parameter
// - sf::RenderWindow: Window object used for rendering graphics, accessed through shared context

