#include "StateGameOver.hpp"
#include "StateManager.hpp"
#include "Window.hpp"
#include <iostream>

StateGameOver::StateGameOver(StateManager* l_stateManager)
    : BaseState(l_stateManager), m_text(m_font), m_instructionText(m_font) {
}

StateGameOver::~StateGameOver() {
}

void StateGameOver::OnCreate() {
    SetTransparent(true); // Set our transparency flag.
    if (!m_font.openFromFile("assets/arial/ARIAL.TTF")) {
        std::cerr << "Error: Failed to load font from assets/arial/ARIAL.TTF" << std::endl;
    }
    m_text.setFont(m_font);
    m_text.setString(sf::String("GAME OVER"));
    m_text.setCharacterSize(18);
    m_text.setStyle(sf::Text::Bold);
    sf::Vector2u windowSize = m_stateMgr->
        GetContext()->m_wind->GetRenderWindow()->getSize();
    sf::FloatRect textRect = m_text.getLocalBounds();
    m_text.setOrigin(sf::Vector2f(textRect.position.x + textRect.size.x / 2.0f,
        textRect.position.y + textRect.size.y / 2.0f));
    m_text.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f - 20.0f));
    
    // Add instruction text below
    sf::Text instructionText(m_font);
    instructionText.setString(sf::String("Press any key to return to menu"));
    instructionText.setCharacterSize(12);
    sf::FloatRect instructionRect = instructionText.getLocalBounds();
    instructionText.setOrigin(sf::Vector2f(instructionRect.position.x + instructionRect.size.x / 2.0f,
        instructionRect.position.y + instructionRect.size.y / 2.0f));
    instructionText.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f + 20.0f));
    
    m_rect.setSize(sf::Vector2f(windowSize));
    m_rect.setPosition(sf::Vector2f(0, 0));
    m_rect.setFillColor(sf::Color(0, 0, 0, 150));
    
    // Register callback for any key press - register common keys
    EventManager* evMgr = m_stateMgr->
        GetContext()->m_eventManager;
    // Register multiple common keys to handle "any key" behavior
    evMgr->AddCallback(StateType::GameOver, "Intro_Continue",  // Spacebar
        &StateGameOver::ReturnToMenu, this);
    evMgr->AddCallback(StateType::GameOver, "Key_Escape",
        &StateGameOver::ReturnToMenu, this);
    evMgr->AddCallback(StateType::GameOver, "Key_P",
        &StateGameOver::ReturnToMenu, this);
    evMgr->AddCallback(StateType::GameOver, "Key_Up",
        &StateGameOver::ReturnToMenu, this);
    evMgr->AddCallback(StateType::GameOver, "Key_Down",
        &StateGameOver::ReturnToMenu, this);
    evMgr->AddCallback(StateType::GameOver, "Key_Left",
        &StateGameOver::ReturnToMenu, this);
    evMgr->AddCallback(StateType::GameOver, "Key_Right",
        &StateGameOver::ReturnToMenu, this);
}

void StateGameOver::OnDestroy() {
    EventManager* evMgr = m_stateMgr->
        GetContext()->m_eventManager;
    evMgr->RemoveCallback(StateType::GameOver, "Intro_Continue");
    evMgr->RemoveCallback(StateType::GameOver, "Key_Escape");
    evMgr->RemoveCallback(StateType::GameOver, "Key_P");
    evMgr->RemoveCallback(StateType::GameOver, "Key_Up");
    evMgr->RemoveCallback(StateType::GameOver, "Key_Down");
    evMgr->RemoveCallback(StateType::GameOver, "Key_Left");
    evMgr->RemoveCallback(StateType::GameOver, "Key_Right");
}

void StateGameOver::Activate() {
}

void StateGameOver::Deactivate() {
}

void StateGameOver::Update(const sf::Time& l_time) {
    (void)l_time; // Suppress unused parameter warning
}

void StateGameOver::Draw() {
    sf::RenderWindow* wind = m_stateMgr->
        GetContext()->m_wind->GetRenderWindow();
    wind->draw(m_rect);
    wind->draw(m_text);
    wind->draw(m_instructionText);
}

void StateGameOver::ReturnToMenu(EventDetails* l_details) {
    (void)l_details; // Suppress unused parameter warning
    m_stateMgr->SwitchTo(StateType::MainMenu);
    m_stateMgr->Remove(StateType::GameOver);
}

