#include "EventManager.hpp"
#include "StateManager.hpp"  // For StateType enum
#include <fstream>
#include <sstream>
#include <iostream>

EventManager::EventManager() : m_hasFocus(true), m_currentState(StateType::Global) {
    LoadBindings();
}

EventManager::~EventManager() {
    for (auto& itr : m_bindings) {
        delete itr.second;
        itr.second = nullptr;
    }
}

bool EventManager::AddBinding(Binding* l_binding) {
    if (m_bindings.find(l_binding->m_name) != m_bindings.end()) {
        return false;
    }
    return m_bindings.emplace(l_binding->m_name, l_binding).second;
}

bool EventManager::RemoveBinding(std::string l_name) {
    auto itr = m_bindings.find(l_name);
    if (itr == m_bindings.end()) {
        return false;
    }
    delete itr->second;
    m_bindings.erase(itr);
    return true;
}

void EventManager::SetFocus(const bool& l_focus) {
    m_hasFocus = l_focus;
}

void EventManager::SetCurrentState(StateType l_state) {
    m_currentState = l_state;
}

bool EventManager::RemoveCallback(StateType l_state, const std::string& l_name) {
    auto itr = m_callbacks.find(l_state);
    if (itr == m_callbacks.end()) { return false; }
    auto itr2 = itr->second.find(l_name);
    if (itr2 == itr->second.end()) { return false; }
    itr->second.erase(l_name);
    return true;
}

/// @brief Handles incoming SFML events and matches them to registered bindings.
/// 
/// This method processes SFML events by iterating through all registered bindings
/// and their associated events. When a match is found:
/// - For keyboard events (KeyDown/KeyUp): Checks if the key code matches the binding's event code
/// - For mouse button events (MButtonDown/MButtonUp): Checks if the button matches and stores mouse position
/// - For MouseWheel events: Stores the wheel delta value
/// - For WindowResized events: Stores the new window dimensions (width, height)
/// - For TextEntered events: Stores the unicode character entered
/// 
/// When a match is found, the binding's event count is incremented and relevant
/// event details are stored in the binding's EventDetails structure.
/// 
/// @param l_event Reference to the SFML event to process
void EventManager::HandleEvent(t_event& l_event) {
    // Map t_event.type to EventType enum
    // t_event.type values match EventType enum values
    EventType eventType = static_cast<EventType>(l_event.type);
    
    // Check if event type is valid
    if (l_event.type < CLOSED || l_event.type > MOUSE_LEFT) {
        return; // Unknown event type
    }
    
    // Debug: log mouse events (key events logged only on match)
    if (eventType == EventType::MouseButtonPressed || eventType == EventType::MouseButtonReleased) {
        std::cout << "[EventManager::HandleEvent] Mouse event type=" << static_cast<int>(eventType) 
                  << " button=" << l_event.mouse.button 
                  << " at (" << l_event.mouse.x << ", " << l_event.mouse.y << ")" << std::endl;
    }
    
    for (auto& b_itr : m_bindings) {
        Binding* bind = b_itr.second;
        for (auto& e_itr : bind->m_events) {
            if (e_itr.first != eventType) {
                continue;
            }
            if (eventType == EventType::KeyPressed ||
                eventType == EventType::KeyReleased) {
                if (e_itr.second.m_code == l_event.keyCode) {
                    std::cout << "[EventManager::HandleEvent] Match! Key code " << l_event.keyCode << " matches binding '" << bind->m_name << "'" << std::endl;
                    bind->m_details.m_keyCode = l_event.keyCode;
                    ++(bind->c);
                    break;
                }
            } else if (eventType == EventType::MouseButtonPressed ||
                       eventType == EventType::MouseButtonReleased) {
                std::cout << "[EventManager] Checking binding '" << bind->m_name 
                          << "' for mouse button " << l_event.mouse.button 
                          << " (binding expects " << e_itr.second.m_code << ")" << std::endl;
                if (e_itr.second.m_code == l_event.mouse.button) {
                    std::cout << "[EventManager] Match! Setting mouse coords to (" 
                              << l_event.mouse.x << ", " << l_event.mouse.y << ")" << std::endl;
                    bind->m_details.m_mouse.x = l_event.mouse.x;
                    bind->m_details.m_mouse.y = l_event.mouse.y;
                    if (bind->m_details.m_keyCode != -1) {
                        bind->m_details.m_keyCode = l_event.mouse.button;
                    }
                    ++(bind->c);
                    break;
                }
            } else {
                // No need for additional checking.
                if (eventType == EventType::MouseWheelScrolled) {
                    bind->m_details.m_mouseWheelDelta = l_event.wheelDelta;
                } else if (eventType == EventType::Resized) {
                    bind->m_details.m_size.x = l_event.window.width;
                    bind->m_details.m_size.y = l_event.window.height;
                } else if (eventType == EventType::TextEntered) {
                    bind->m_details.m_textEntered = l_event.unicode;
                }
                ++(bind->c);
            }
        }
    }
}

void EventManager::Update() {
    if (!m_hasFocus) {
        return;
    }
    for (auto& b_itr : m_bindings) {
        Binding* bind = b_itr.second;
        for (auto& e_itr : bind->m_events) {
            switch (e_itr.first) {
                case(EventType::Keyboard):
                    // TODO: Continuous keyboard polling - needs graphics-agnostic implementation
                    // if (sf::Keyboard::isKeyPressed(
                    //     sf::Keyboard::Key(e_itr.second.m_code))) {
                    //     std::cout << "Keyboard key " << e_itr.second.m_code << " is currently pressed (continuous check)" << std::endl;
                    //     if (bind->m_details.m_keyCode != -1) {
                    //         bind->m_details.m_keyCode = e_itr.second.m_code;
                    //     }
                    //     ++(bind->c);
                    // }
                    break;
                case(EventType::Mouse):
                    // TODO: Continuous mouse polling - needs graphics-agnostic implementation
                    // if (sf::Mouse::isButtonPressed(
                    //     sf::Mouse::Button(e_itr.second.m_code))) {
                    //     // std::cout << "Mouse button " << e_itr.second.m_code << " is currently pressed (continuous check)" << std::endl;
                    //     if (bind->m_details.m_keyCode != -1) {
                    //         bind->m_details.m_keyCode = e_itr.second.m_code;
                    //     }
                    //     ++(bind->c);
                    // }
                    break;
                case(EventType::Joystick):
                    // Up for expansion.
                    break;
                default:
                    // Other event types are handled in HandleEvent, not here
                    break;
            }
        }
        if (static_cast<int>(bind->m_events.size()) == bind->c) {
            std::cout << "[EventManager::Update] Binding '" << bind->m_name << "' triggered! All " << bind->c << " events matched." << std::endl;
            std::cout << "[EventManager::Update] Current state: " << static_cast<int>(m_currentState) << std::endl;
            
            // Check callbacks for current state
            auto stateCallbacks = m_callbacks.find(m_currentState);
            if (stateCallbacks != m_callbacks.end()) {
                std::cout << "[EventManager::Update] Found callbacks for current state, looking for '" << bind->m_name << "'" << std::endl;
                auto callItr = stateCallbacks->second.find(bind->m_name);
                if (callItr != stateCallbacks->second.end()) {
                    std::cout << "[EventManager::Update] Calling callback for '" << bind->m_name << "'" << std::endl;
                    callItr->second(&bind->m_details);
                } else {
                    std::cout << "[EventManager::Update] No callback found for '" << bind->m_name << "' in current state" << std::endl;
                }
            } else {
                std::cout << "[EventManager::Update] No callbacks registered for current state" << std::endl;
            }
            
            // Check global callbacks (StateType::Global - always active regardless of state)
            auto otherCallbacks = m_callbacks.find(StateType::Global);
            if (otherCallbacks != m_callbacks.end()) {
                std::cout << "[EventManager::Update] Checking global callbacks for '" << bind->m_name << "'" << std::endl;
                auto callItr = otherCallbacks->second.find(bind->m_name);
                if (callItr != otherCallbacks->second.end()) {
                    std::cout << "[EventManager::Update] Calling global callback for '" << bind->m_name << "'" << std::endl;
                    callItr->second(&bind->m_details);
                }
            }
        }
        bind->c = 0;
        bind->m_details.Clear();
    }
}

void EventManager::LoadBindings() {
    std::string delimiter = ":";
    std::ifstream bindings;
    bindings.open("keys.cfg");
    if (!bindings.is_open()) {
        std::cout << "! Failed loading keys.cfg." << std::endl;
        return;
    }
    std::cout << "[EventManager] Loading bindings from keys.cfg..." << std::endl;
    
    std::string line;
    while (std::getline(bindings, line)) {
        if (line.empty()) {
            continue;
        }
        std::stringstream keystream(line);
        std::string callbackName;
        keystream >> callbackName;
        if (callbackName.empty()) {
            continue;
        }
        Binding* bind = new Binding(callbackName);
        
        while (!keystream.eof()) {
            std::string keyval;
            keystream >> keyval;
            if (keyval.empty()) {
                break;
            }
            int start = 0;
            std::string::size_type end = keyval.find(delimiter);
            if (end == std::string::npos) {
                delete bind;
                bind = nullptr;
                break;
            }
            try {
                EventType type = EventType(
                    stoi(keyval.substr(start, end - start)));
                std::string codeStr = keyval.substr(end + delimiter.length());
                std::string::size_type nextDelim = codeStr.find(delimiter);
                int code = stoi(nextDelim == std::string::npos ? codeStr : codeStr.substr(0, nextDelim));
                EventInfo eventInfo;
                eventInfo.m_code = code;
                bind->BindEvent(type, eventInfo);
            } catch (...) {
                delete bind;
                bind = nullptr;
                break;
            }
        }
        
        if (bind && !AddBinding(bind)) {
            std::cout << "[EventManager] Failed to add binding: " << bind->m_name << std::endl;
            delete bind;
        } else if (bind) {
            std::cout << "[EventManager] Loaded binding: " << bind->m_name << " with " << bind->m_events.size() << " event(s)" << std::endl;
        }
    }
    bindings.close();
    std::cout << "[EventManager] Loaded " << m_bindings.size() << " bindings total" << std::endl;
}

