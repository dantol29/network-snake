#include "StateMushroom.hpp"
#include "StateManager.hpp"
#include "Window.hpp"
#include <iostream>

StateMushroom::StateMushroom(StateManager* l_stateManager)
    : BaseState(l_stateManager), m_sprite(m_texture) {
}

StateMushroom::~StateMushroom() {
}

void StateMushroom::OnCreate() {
    if (!m_texture.loadFromFile("assets/Mushroom.png")) {
        std::cerr << "Error: Failed to load texture from assets/Mushroom.png" << std::endl;
    }
    m_sprite.setTexture(m_texture, true);
    m_sprite.setPosition(sf::Vector2f(0, 0));
    m_increment = sf::Vector2f(400.0f, 400.0f);
    
    EventManager* evMgr = m_stateMgr->
        GetContext()->m_eventManager;
    evMgr->AddCallback(StateType::Mushroom, "Key_Escape",
        &StateMushroom::MainMenu, this);
    evMgr->AddCallback(StateType::Mushroom, "Key_P",
        &StateMushroom::Pause, this);
}

void StateMushroom::OnDestroy() {
    EventManager* evMgr = m_stateMgr->
        GetContext()->m_eventManager;
    evMgr->RemoveCallback(StateType::Mushroom, "Key_Escape");
    evMgr->RemoveCallback(StateType::Mushroom, "Key_P");
}

void StateMushroom::Activate() {
}

void StateMushroom::Deactivate() {
}

void StateMushroom::Update(const sf::Time& l_time) {
    sf::Vector2u l_windSize = m_stateMgr->GetContext()->
        m_wind->GetWindowSize();
    sf::Vector2u l_textSize = m_texture.getSize();
    
    if ((m_sprite.getPosition().x > l_windSize.x - l_textSize.x && m_increment.x > 0) ||
        (m_sprite.getPosition().x < 0 && m_increment.x < 0))
    {
        m_increment.x = -m_increment.x;
    }
    if ((m_sprite.getPosition().y > l_windSize.y - l_textSize.y && m_increment.y > 0) ||
        (m_sprite.getPosition().y < 0 && m_increment.y < 0))
    {
        m_increment.y = -m_increment.y;
    }
    m_sprite.setPosition(sf::Vector2f(
        m_sprite.getPosition().x + (m_increment.x * l_time.asSeconds()),
        m_sprite.getPosition().y + (m_increment.y * l_time.asSeconds())));
}

void StateMushroom::Draw() {
    m_stateMgr->GetContext()->m_wind->
        GetRenderWindow()->draw(m_sprite);
}

void StateMushroom::MainMenu(EventDetails* l_details) {
    (void)l_details; // Suppress unused parameter warning
    m_stateMgr->SwitchTo(StateType::MainMenu);
}

void StateMushroom::Pause(EventDetails* l_details) {
    (void)l_details; // Suppress unused parameter warning
    m_stateMgr->SwitchTo(StateType::Paused);
}

