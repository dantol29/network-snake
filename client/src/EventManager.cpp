#include "EventManager.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

EventManager::EventManager()
    : has_focus_(true), current_state_(StateType::Global) {
  LoadBindings();
}

EventManager::~EventManager() {
  for (auto &it : bindings_) {
    delete it.second;
    it.second = nullptr;
  }
}

bool EventManager::AddBinding(Binding *binding) {
  if (bindings_.find(binding->name_) != bindings_.end()) {
    return false;
  }
  return bindings_.emplace(binding->name_, binding).second;
}

bool EventManager::RemoveBinding(std::string name) {
  auto it = bindings_.find(name);
  if (it == bindings_.end()) {
    return false;
  }
  delete it->second;
  bindings_.erase(it);
  return true;
}

void EventManager::SetFocus(const bool &has_focus) { has_focus_ = has_focus; }

void EventManager::SetCurrentState(StateType state) { current_state_ = state; }

bool EventManager::RemoveCallback(StateType state, const std::string &name) {
  auto it = callbacks_.find(state);
  if (it == callbacks_.end()) {
    return false;
  }
  auto it2 = it->second.find(name);
  if (it2 == it->second.end()) {
    return false;
  }
  it->second.erase(name);
  return true;
}

void EventManager::HandleEvent(t_event &event) {
  // Map t_event.type to EventType enum
  // t_event.type values match EventType enum values
  EventType eventType = static_cast<EventType>(event.type);

  // Check if event type is valid
  if (event.type < CLOSED || event.type > MOUSE_LEFT) {
    return; // Unknown event type
  }

  // Debug: log mouse events (key events logged only on match)
  if (eventType == EventType::MouseButtonPressed ||
      eventType == EventType::MouseButtonReleased) {
    std::cout << "[EventManager::HandleEvent] Mouse event type="
              << static_cast<int>(eventType) << " button=" << event.mouse.button
              << " at (" << event.mouse.x << ", " << event.mouse.y << ")"
              << std::endl;
  }

  for (auto &binding : bindings_) {
    Binding *binding_ptr = binding.second;
    for (auto &event_pair : binding_ptr->events_) {
      if (event_pair.first != eventType) {
        continue;
      }
      if (eventType == EventType::KeyPressed ||
          eventType == EventType::KeyReleased) {
        if (event_pair.second.code_ == event.keyCode) {
          std::cout << "[EventManager::HandleEvent] Match! Key code "
                    << event.keyCode << " matches binding '"
                    << binding_ptr->name_ << "'" << std::endl;
          binding_ptr->details_.key_code_ = event.keyCode;
          ++(binding_ptr->c);
          break;
        }
      } else if (eventType == EventType::MouseButtonPressed ||
                 eventType == EventType::MouseButtonReleased) {
        std::cout << "[EventManager] Checking binding '" << binding_ptr->name_
                  << "' for mouse button " << event.mouse.button
                  << " (binding expects " << event_pair.second.code_ << ")"
                  << std::endl;
        if (event_pair.second.code_ == event.mouse.button) {
          std::cout << "[EventManager] Match! Setting mouse coords to ("
                    << event.mouse.x << ", " << event.mouse.y << ")"
                    << std::endl;
          binding_ptr->details_.mouse_.x = event.mouse.x;
          binding_ptr->details_.mouse_.y = event.mouse.y;
          if (binding_ptr->details_.key_code_ != -1) {
            binding_ptr->details_.key_code_ = event.mouse.button;
          }
          ++(binding_ptr->c);
          break;
        }
      } else {
        // No need for additional checking.
        if (eventType == EventType::MouseWheelScrolled) {
          binding_ptr->details_.mouse_wheel_delta_ = event.wheelDelta;
        } else if (eventType == EventType::Resized) {
          binding_ptr->details_.size_.x = event.window.width;
          binding_ptr->details_.size_.y = event.window.height;
        } else if (eventType == EventType::TextEntered) {
          binding_ptr->details_.text_entered_ = event.unicode;
        }
        ++(binding_ptr->c);
      }
    }
  }
}

void EventManager::Update() {
  if (!has_focus_) {
    return;
  }
  for (auto &binding : bindings_) {
    Binding *binding_ptr = binding.second;
    for (auto &event_pair : binding_ptr->events_) {
      switch (event_pair.first) {
      case (EventType::Keyboard):
        // TODO: Continuous keyboard polling - needs graphics-agnostic
        // implementation if (sf::Keyboard::isKeyPressed(
        //     sf::Keyboard::Key(event_pair.second.code_))) {
        //     std::cout << "Keyboard key " << event_pair.second.code_ << " is
        //     currently pressed (continuous check)" << std::endl; if
        //     (binding_ptr->details_.key_code_ != -1) {
        //         binding_ptr->details_.key_code_ = event_pair.second.code_;
        //     }
        //     ++(binding_ptr->c);
        // }
        break;
      case (EventType::Mouse):
        // TODO: Continuous mouse polling - needs graphics-agnostic
        // implementation if (sf::Mouse::isButtonPressed(
        //     sf::Mouse::Button(event_pair.second.code_))) {
        //     // std::cout << "Mouse button " << event_pair.second.code_ << "
        //     is currently pressed (continuous check)" << std::endl; if
        //     (binding_ptr->details_.key_code_ != -1) {
        //         binding_ptr->details_.key_code_ = event_pair.second.code_;
        //     }
        //     ++(binding_ptr->c);
        // }
        break;
      case (EventType::Joystick):
        // Up for expansion.
        break;
      default:
        // Other event types are handled in HandleEvent, not here
        break;
      }
    }
    if (static_cast<int>(binding_ptr->events_.size()) == binding_ptr->c) {
      std::cout << "[EventManager::Update] Binding '" << binding_ptr->name_
                << "' triggered! All " << binding_ptr->c << " events matched."
                << std::endl;
      std::cout << "[EventManager::Update] Current state: "
                << static_cast<int>(current_state_) << std::endl;

      // Check callbacks for current state
      auto state_callbacks = callbacks_.find(current_state_);
      if (state_callbacks != callbacks_.end()) {
        std::cout << "[EventManager::Update] Found callbacks for current "
                     "state, looking for '"
                  << binding_ptr->name_ << "'" << std::endl;
        auto call_it = state_callbacks->second.find(binding_ptr->name_);
        if (call_it != state_callbacks->second.end()) {
          std::cout << "[EventManager::Update] Calling callback for '"
                    << binding_ptr->name_ << "'" << std::endl;
          call_it->second(&binding_ptr->details_);
        } else {
          std::cout << "[EventManager::Update] No callback found for '"
                    << binding_ptr->name_ << "' in current state" << std::endl;
        }
      } else {
        std::cout << "[EventManager::Update] No callbacks registered for "
                     "current state"
                  << std::endl;
      }

      // Check global callbacks (StateType(0) - always active regardless of
      // state)
      auto other_callbacks = callbacks_.find(StateType(0));
      if (other_callbacks != callbacks_.end()) {
        std::cout << "[EventManager::Update] Checking global callbacks for '"
                  << binding_ptr->name_ << "'" << std::endl;
        auto call_it = other_callbacks->second.find(binding_ptr->name_);
        if (call_it != other_callbacks->second.end()) {
          std::cout << "[EventManager::Update] Calling global callback for '"
                    << binding_ptr->name_ << "'" << std::endl;
          call_it->second(&binding_ptr->details_);
        }
      }
    }
    binding_ptr->c = 0;
    binding_ptr->details_.Clear();
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
    std::stringstream key_stream(line);
    std::string callbackName;
    key_stream >> callbackName;
    if (callbackName.empty()) {
      continue;
    }
    Binding *binding = new Binding(callbackName);

    while (!key_stream.eof()) {
      std::string key_val;
      key_stream >> key_val;
      if (key_val.empty()) {
        break;
      }
      int start = 0;
      std::string::size_type end = key_val.find(delimiter);
      if (end == std::string::npos) {
        delete binding;
        binding = nullptr;
        break;
      }
      try {
        EventType type = EventType(stoi(key_val.substr(start, end - start)));
        std::string codeStr = key_val.substr(end + delimiter.length());
        std::string::size_type nextDelim = codeStr.find(delimiter);
        int code =
            stoi(nextDelim == std::string::npos ? codeStr
                                                : codeStr.substr(0, nextDelim));
        EventInfo eventInfo;
        eventInfo.code_ = code;
        binding->BindEvent(type, eventInfo);
      } catch (...) {
        delete binding;
        binding = nullptr;
        break;
      }
    }

    if (binding && !AddBinding(binding)) {
      std::cout << "[EventManager] Failed to add binding: " << binding->name_
                << std::endl;
      delete binding;
    } else if (binding) {
      std::cout << "[EventManager] Loaded binding: " << binding->name_
                << " with " << binding->events_.size() << " event(s)"
                << std::endl;
    }
  }
  bindings.close();
  std::cout << "[EventManager] Loaded " << bindings_.size() << " bindings total"
            << std::endl;
}
