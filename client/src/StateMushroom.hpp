#pragma once

#include "BaseState.hpp"
#include "EventManager.hpp"
#include <SFML/Graphics.hpp>

class StateMushroom : public BaseState {
public:
    StateMushroom(StateManager* l_stateManager);
    ~StateMushroom();

    void OnCreate() override;
    void OnDestroy() override;
    void Activate() override;
    void Deactivate() override;
    void Update(const sf::Time& l_time) override;
    void Draw() override;

    void MainMenu(EventDetails* l_details);
    void Pause(EventDetails* l_details);

private:
    sf::Texture m_texture;
    sf::Sprite m_sprite;
    sf::Vector2f m_increment;
};

// SFML types used in this file:
// - sf::Texture: Image texture, used for m_texture
// - sf::Sprite: Drawable sprite, used for m_sprite
// - sf::Vector2f: 2D vector with float components, used for m_increment (velocity)
// - sf::Time: Time duration type, used in Update() method parameter
// - sf::RenderWindow: Window object used for rendering graphics, accessed through shared context

