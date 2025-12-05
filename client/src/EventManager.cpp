#include "EventManager.hpp"
#include <fstream>
#include <sstream>

EventManager::EventManager()
    : has_focus_(true), current_state_(StateType::Global) {
  LoadTargetEventBindings();
}

EventManager::~EventManager() {
  for (auto &it : bindings_) {
    delete it.second;
    it.second = nullptr;
  }
}

bool EventManager::AddBinding(TargetEventBindingState *binding) {
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
  TargetEventType eventType = static_cast<TargetEventType>(event.type);

  if (event.type < CLOSED || event.type > MOUSE_LEFT) {
    return;
  }

  for (auto &binding : bindings_) {
    TargetEventBindingState *binding_ptr = binding.second;
    for (auto &event_pair : binding_ptr->events_) {
      if (event_pair.first != eventType) {
        continue;
      }
      if (eventType == TargetEventType::KeyPressed ||
          eventType == TargetEventType::KeyReleased) {
        if (event_pair.second.code_ == event.keyCode) {
          binding_ptr->details_.key_code_ = event.keyCode;
          ++(binding_ptr->matched_count_);
          break;
        }
      } else if (eventType == TargetEventType::MouseButtonPressed ||
                 eventType == TargetEventType::MouseButtonReleased) {
        if (event_pair.second.code_ == event.mouse.button) {
          binding_ptr->details_.mouse_position_.x = event.mouse.x;
          binding_ptr->details_.mouse_position_.y = event.mouse.y;
          if (binding_ptr->details_.key_code_ != -1) {
            binding_ptr->details_.key_code_ = event.mouse.button;
          }
          ++(binding_ptr->matched_count_);
          break;
        }
      } else {
        if (eventType == TargetEventType::MouseWheelScrolled) {
          binding_ptr->details_.mouse_wheel_delta_ = event.wheelDelta;
        } else if (eventType == TargetEventType::Resized) {
          binding_ptr->details_.window_size_.x = event.window.width;
          binding_ptr->details_.window_size_.y = event.window.height;
        } else if (eventType == TargetEventType::TextEntered) {
          binding_ptr->details_.text_entered_ = event.unicode;
        }
        ++(binding_ptr->matched_count_);
      }
    }
  }
}

void EventManager::Update() {
  if (!has_focus_) {
    return;
  }
  for (auto &binding : bindings_) {
    TargetEventBindingState *binding_ptr = binding.second;
    for (auto &event_pair : binding_ptr->events_) {
      switch (event_pair.first) {
      case (TargetEventType::Joystick):
        break;
      default:
        break;
      }
    }
    if (static_cast<int>(binding_ptr->events_.size()) ==
        binding_ptr->matched_count_) {
      auto state_callbacks = callbacks_.find(current_state_);
      if (state_callbacks != callbacks_.end()) {
        auto call_it = state_callbacks->second.find(binding_ptr->name_);
        if (call_it != state_callbacks->second.end()) {
          call_it->second(&binding_ptr->details_);
        }
      }

      auto other_callbacks = callbacks_.find(StateType(0));
      if (other_callbacks != callbacks_.end()) {
        auto call_it = other_callbacks->second.find(binding_ptr->name_);
        if (call_it != other_callbacks->second.end()) {
          call_it->second(&binding_ptr->details_);
        }
      }
    }
    binding_ptr->matched_count_ = 0;
    binding_ptr->details_.Clear();
  }
}

void EventManager::LoadTargetEventBindings() {
  std::string delimiter = ":";
  std::ifstream bindings;
  bindings.open("keys.cfg");
  if (!bindings.is_open()) {
    return;
  }

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
    TargetEventBindingState *binding =
        new TargetEventBindingState(callbackName);

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
        TargetEventType type =
            TargetEventType(stoi(key_val.substr(start, end - start)));
        std::string codeStr = key_val.substr(end + delimiter.length());
        std::string::size_type nextDelim = codeStr.find(delimiter);
        int code =
            stoi(nextDelim == std::string::npos ? codeStr
                                                : codeStr.substr(0, nextDelim));
        TargetEventCode event_code;
        event_code.code_ = code;
        binding->AddTargetEvent(type, event_code);
      } catch (...) {
        delete binding;
        binding = nullptr;
        break;
      }
    }

    if (binding && !AddBinding(binding)) {
      delete binding;
    }
  }
  bindings.close();
}
