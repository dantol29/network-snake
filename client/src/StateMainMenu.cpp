#include "StateMainMenu.hpp"
#include "StateManager.hpp"
#include "Window.hpp"
#include <iostream>

StateMainMenu::StateMainMenu(StateManager* l_stateManager)
    : BaseState(l_stateManager), m_text(m_font), m_labels{m_font, m_font, m_font, m_font} {
}

StateMainMenu::~StateMainMenu() {
}

void StateMainMenu::OnCreate() {
    if (!m_font.openFromFile("assets/arial/ARIAL.TTF")) {
        std::cerr << "Error: Failed to load font from assets/arial/ARIAL.TTF" << std::endl;
    }
    m_text.setFont(m_font);
    m_text.setString(sf::String("MAIN MENU:"));
    m_text.setCharacterSize(18);
    sf::FloatRect textRect = m_text.getLocalBounds();
    m_text.setOrigin(sf::Vector2f(textRect.position.x + textRect.size.x / 2.0f,
        textRect.position.y + textRect.size.y / 2.0f));
    m_text.setPosition(sf::Vector2f(400, 100));
    
    m_buttonSize = sf::Vector2f(300.0f, 32.0f);
    m_buttonPos = sf::Vector2f(400, 200);
    m_buttonPadding = 4; // 4px.
    
    std::string str[4];
    str[0] = "PLAY";
    str[1] = "MUSHROOM";
    str[2] = "CREDITS";
    str[3] = "EXIT";
    
    for (int i = 0; i < 4; ++i) {
        sf::Vector2f buttonPosition(m_buttonPos.x, m_buttonPos.y +
            (i * (m_buttonSize.y + m_buttonPadding)));
        m_rects[i].setSize(m_buttonSize);
        m_rects[i].setFillColor(sf::Color::Red);
        m_rects[i].setOrigin(sf::Vector2f(m_buttonSize.x / 2.0f,
            m_buttonSize.y / 2.0f));
        m_rects[i].setPosition(buttonPosition);
        m_labels[i].setFont(m_font);
        m_labels[i].setString(sf::String(str[i]));
        m_labels[i].setCharacterSize(12);
        sf::FloatRect rect = m_labels[i].getLocalBounds();
        m_labels[i].setOrigin(sf::Vector2f(rect.position.x + rect.size.x / 2.0f,
            rect.position.y + rect.size.y / 2.0f));
        m_labels[i].setPosition(buttonPosition);
    }
    
    EventManager* evMgr = m_stateMgr->
        GetContext()->m_eventManager;
    evMgr->AddCallback(StateType::MainMenu, "Mouse_Left",
        &StateMainMenu::MouseClick, this);
}

void StateMainMenu::OnDestroy() {
    EventManager* evMgr = m_stateMgr->
        GetContext()->m_eventManager;
    evMgr->RemoveCallback(StateType::MainMenu, "Mouse_Left");
}

void StateMainMenu::Activate() {
    if (m_stateMgr->HasState(StateType::Game)
        && m_labels[0].getString() == "PLAY")
    {
        m_labels[0].setString(sf::String("RESUME"));
        sf::FloatRect rect = m_labels[0].getLocalBounds();
        m_labels[0].setOrigin(sf::Vector2f(rect.position.x + rect.size.x / 2.0f,
            rect.position.y + rect.size.y / 2.0f));
    }
}

void StateMainMenu::Deactivate() {
}

void StateMainMenu::Update(const sf::Time& l_time) {
    (void)l_time; // Suppress unused parameter warning
}

void StateMainMenu::Draw() {
    sf::RenderWindow* window = m_stateMgr->GetContext()->
        m_wind->GetRenderWindow();
    window->draw(m_text);
    for (int i = 0; i < 4; ++i) {
        window->draw(m_rects[i]);
        window->draw(m_labels[i]);
    }
}

void StateMainMenu::MouseClick(EventDetails* l_details) {
    sf::Vector2i mousePos = l_details->m_mouse;
    std::cout << "MouseClick called! Mouse position: (" << mousePos.x << ", " << mousePos.y << ")" << std::endl;
    
    float halfX = m_buttonSize.x / 2.0f;
    float halfY = m_buttonSize.y / 2.0f;
    for (int i = 0; i < 4; ++i) {
        sf::Vector2f buttonPos = m_rects[i].getPosition();
        float left = buttonPos.x - halfX;
        float right = buttonPos.x + halfX;
        float top = buttonPos.y - halfY;
        float bottom = buttonPos.y + halfY;
        
        std::cout << "Button " << i << " bounds: left=" << left << ", right=" << right 
                  << ", top=" << top << ", bottom=" << bottom << std::endl;
        
        if (mousePos.x >= left && mousePos.x <= right &&
            mousePos.y >= top && mousePos.y <= bottom)
        {
            std::cout << "Button " << i << " clicked!" << std::endl;
            if (i == 0) {
                // PLAY button - switch to actual snake game
                std::cout << "Switching to Game state (snake game)" << std::endl;
                m_stateMgr->SwitchTo(StateType::Game);
            } else if (i == 1) {
                // MUSHROOM button - switch to bouncing mushroom demo
                std::cout << "Switching to Mushroom state (demo)" << std::endl;
                m_stateMgr->SwitchTo(StateType::Mushroom);
            } else if (i == 2) {
                // Credits state.
            } else if (i == 3) {
                std::cout << "Closing window" << std::endl;
                m_stateMgr->GetContext()->m_wind->Close();
            }
        }
    }
}

