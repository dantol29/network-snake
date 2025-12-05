#include "StateIntro.hpp"
#include "StateManager.hpp"
#include "Window.hpp"
#include <iostream>
#include <chrono> // For timing verification (using static variable, not member)

StateIntro::StateIntro(StateManager* l_stateManager)
    : BaseState(l_stateManager), m_introSprite(m_introTexture), m_text(m_font), 
      m_timePassed(0.0f) /*, m_startTime() */ { // COMMENTED OUT: m_startTime initialization
}

StateIntro::~StateIntro() {
}

void StateIntro::OnCreate() {
    // Initialize time tracking
    m_timePassed = 0.0f;
    
    // Get window size for positioning
    sf::Vector2u windowSize = m_stateMgr->GetContext()->
        m_wind->GetRenderWindow()->getSize();

    // Load and setup intro sprite
    if (!m_introTexture.loadFromFile("assets/intro.png")) {
        std::cerr << "Error: Failed to load texture from assets/intro.png" << std::endl;
    } else {
        std::cout << "Intro texture loaded successfully. Size: " 
                  << m_introTexture.getSize().x << "x" << m_introTexture.getSize().y << std::endl;
        // Set texture with resetRect=true to update sprite bounds after loading
        m_introSprite.setTexture(m_introTexture, true);
        sf::Vector2u textureSize = m_introTexture.getSize();
        m_introSprite.setOrigin(sf::Vector2f(textureSize.x / 2.0f,
            textureSize.y / 2.0f));
        m_introSprite.setPosition(sf::Vector2f(windowSize.x / 2.0f, 0));
        std::cout << "Intro sprite positioned at (" << windowSize.x / 2.0f << ", 0)" << std::endl;
        std::cout << "Texture size: " << textureSize.x << "x" << textureSize.y << std::endl;
        // Verify sprite bounds
        sf::FloatRect bounds = m_introSprite.getGlobalBounds();
        std::cout << "Sprite global bounds: (" << bounds.position.x << ", " << bounds.position.y 
                  << ") size: " << bounds.size.x << "x" << bounds.size.y << std::endl;
    }

    // Load and setup text
    if (!m_font.openFromFile("assets/arial/ARIAL.TTF")) {
        std::cerr << "Error: Failed to load font from assets/arial/ARIAL.TTF" << std::endl;
    }
    m_text.setFont(m_font);
    m_text.setString({ "Press SPACE to continue" });
    m_text.setCharacterSize(15);
    sf::FloatRect textRect = m_text.getLocalBounds();
    m_text.setOrigin(sf::Vector2f(textRect.position.x + textRect.size.x / 2.0f,
        textRect.position.y + textRect.size.y / 2.0f));
    m_text.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f));

    // Bind Spacebar key to Continue method
    // Get event manager through shared context and register callback with state type
    EventManager* evMgr = m_stateMgr->
        GetContext()->m_eventManager;
    evMgr->AddCallback(StateType::Intro, "Intro_Continue",
        &StateIntro::Continue, this);
}

void StateIntro::OnDestroy() {
    EventManager* evMgr = m_stateMgr->
        GetContext()->m_eventManager;
    evMgr->RemoveCallback(StateType::Intro, "Intro_Continue");
}

void StateIntro::Activate() {
}

void StateIntro::Deactivate() {
}

void StateIntro::Update(const sf::Time& l_time) {
        m_timePassed += l_time.asSeconds();
    
    // COMMENTED OUT: Timing verification with chrono member variable - caused AddressSanitizer heap buffer overflow
    // Verify timing with real clock (for debugging)
    // auto now = std::chrono::steady_clock::now();
    // auto realElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime).count() / 1000.0f;
    // 
    // // Log comparison every second
    // static float lastLogTime = 0.0f;
    // if (m_timePassed - lastLogTime >= 1.0f) {
    //     std::cout << "SFML time: " << m_timePassed << "s, Real time: " << realElapsed 
    //               << "s, Delta: " << l_time.asSeconds() << "s" << std::endl;
    //     lastLogTime = m_timePassed;
    // }
    
    // Simple timing verification using static variable (doesn't change class layout)
    static bool firstCall = true;
    static std::chrono::steady_clock::time_point startTime;
    if (firstCall) {
        startTime = std::chrono::steady_clock::now();
        firstCall = false;
    }
    
    // Log comparison every second
    static float lastLogTime = 0.0f;
    if (m_timePassed - lastLogTime >= 1.0f) {
        auto now = std::chrono::steady_clock::now();
        auto realElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count() / 1000.0f;
        std::cout << "SFML time: " << m_timePassed << "s, Real time: " << realElapsed 
                  << "s, Delta: " << l_time.asSeconds() << "s" << std::endl;
        lastLogTime = m_timePassed;
    }
    
    if (m_timePassed < 5.0f) { // Less than five seconds - sprite animates down
        sf::Vector2f currentPos = m_introSprite.getPosition();
        m_introSprite.setPosition(sf::Vector2f(currentPos.x,
            currentPos.y + (48 * l_time.asSeconds())));
    }
    // After 5 seconds, sprite stops moving and text becomes available
}

void StateIntro::Draw() {
    sf::RenderWindow* window = m_stateMgr->
        GetContext()->m_wind->GetRenderWindow();
    window->draw(m_introSprite);
    if (m_timePassed >= 5.0f) {
        window->draw(m_text);
    }
}

void StateIntro::Continue(EventDetails* l_details) {
    (void)l_details; // Suppress unused parameter warning
    if (m_timePassed >= 5.0f) {
        m_stateMgr->SwitchTo(StateType::MainMenu);
        m_stateMgr->Remove(StateType::Intro);
    }
}

// SFML types and functions used in this file:
// - sf::Texture::loadFromFile(): Loads texture from file
// - sf::Texture::getSize(): Gets texture dimensions
// - sf::Sprite::setTexture(): Sets the texture for the sprite
// - sf::Sprite::setOrigin(): Sets the origin point of the sprite
// - sf::Sprite::setPosition(): Sets the position of the sprite
// - sf::Sprite::getPosition(): Gets the current position of the sprite
// - sf::Font::openFromFile(): Loads font from file
// - sf::Text::setFont(): Sets the font for the text
// - sf::Text::setString(): Sets the text string
// - sf::Text::setCharacterSize(): Sets the character size
// - sf::Text::getLocalBounds(): Gets the local bounding rectangle
// - sf::Text::setOrigin(): Sets the origin point of the text
// - sf::Text::setPosition(): Sets the position of the text
// - sf::Time::asSeconds(): Converts time to seconds (float)
// - sf::Vector2u: 2D vector with unsigned integer components
// - sf::FloatRect: Rectangle with float components
// - sf::RenderWindow::draw(): Draws a drawable object
// - sf::RenderWindow::getSize(): Gets the window size

