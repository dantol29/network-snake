#include "EventManager.hpp"
#include <fstream>
#include <sstream>

EventManager::EventManager() : has_focus_(true), current_state_(StateType::Global) {
  LoadTargetEventBindings();
}

bool EventManager::AddEvent(std::unique_ptr<EventState> event) {
  if (eventStates_.find(event->name_) != eventStates_.end())
    return false;

  return eventStates_.emplace(event->name_, std::move(event)).second;
}

bool EventManager::RemoveEvent(std::string name) {
  auto it = eventStates_.find(name);
  if (it == eventStates_.end())
    return false;

  eventStates_.erase(it);
  return true;
}

StateType EventManager::getCurrentState() const { return current_state_; }

void EventManager::SetFocus(const bool& has_focus) { has_focus_ = has_focus; }

void EventManager::SetCurrentState(StateType state) { current_state_ = state; }

bool EventManager::RemoveCallback(StateType state, const std::string& name) {
  auto it = callbacks_.find(state);
  if (it == callbacks_.end())
    return false;

  auto it2 = it->second.find(name);
  if (it2 == it->second.end())
    return false;

  it->second.erase(name);
  return true;
}

void EventManager::HandleEvent(t_event& event) {
  if (event.type < CLOSED || event.type > MOUSE_LEFT)
    return;

  lastEvent_ = event;
  EventType eventType = static_cast<EventType>(event.type);

  for (auto& eventStatePtr : eventStates_) {
    EventState* eventState = eventStatePtr.second.get();
    for (auto& eventPair : eventState->events_) {
      if (eventPair.first != eventType)
        continue;

      switch (eventType) {
      case EventType::KeyPressed:
      case EventType::KeyReleased:
        if (eventPair.second == event.keyCode)
          ++eventState->matchedEvents_;
        break;
      case EventType::MouseButtonPressed:
      case EventType::MouseButtonReleased:
        if (eventPair.second == event.mouse.button)
          ++eventState->matchedEvents_;
        break;
      case EventType::MouseWheelScrolled:
      case EventType::Resized:
      case EventType::TextEntered:
        ++eventState->matchedEvents_;
        break;
      default:
        std::cout << "Unknown event type: " << (int)eventType << '\n';
      }
    }
  }
}

void EventManager::Update() {
  if (!has_focus_)
    return;

  for (auto& eventStatePtr : eventStates_) {
    EventState* eventState = eventStatePtr.second.get();
    if (eventState->events_.size() == eventState->matchedEvents_) {
      // Collect all callbacks registered for the current state
      auto stateCallbacks = callbacks_.find(current_state_);
      if (stateCallbacks != callbacks_.end()) {
        // Find callback with the same name
        auto call_it = stateCallbacks->second.find(eventState->name_);
        if (call_it != stateCallbacks->second.end())
          call_it->second(&lastEvent_);
      }

      // Check for global callbacks - always active
      auto globalCallbacks = callbacks_.find(StateType(0));
      if (globalCallbacks != callbacks_.end()) {
        auto call_it = globalCallbacks->second.find(eventState->name_);
        if (call_it != globalCallbacks->second.end())
          call_it->second(&lastEvent_);
      }
    }
    eventState->matchedEvents_ = 0;
  }
}

// FILE SYNTAX
// EVENT_NAME EVENT_TYPE:EVENT_CODE
// if needed: add support for multiple EVENT_TYPE:EVENT_CODE per EVENT_NAME
void EventManager::LoadTargetEventBindings() {
  std::ifstream bindings;

  bindings.open("keys.cfg");
  if (!bindings.is_open())
    return;

  std::string line;
  while (std::getline(bindings, line)) {
    // skip commented lines
    if (line.find('#') != std::string::npos)
      continue;

    size_t spacePos = line.find_first_of(' ');
    if (spacePos == std::string::npos || spacePos < 1)
      continue;

    auto event = std::make_unique<EventState>(line.substr(0, spacePos));

    std::string remainingLine = line.substr(spacePos + 1);
    size_t columnPos = remainingLine.find_first_of(':');
    if (columnPos == std::string::npos)
      continue;

    try {
      auto eventType = EventType(stoi(remainingLine.substr(0, columnPos)));
      size_t eventCode = stoi(remainingLine.substr(columnPos + 1));
      event->AddEvent(eventType, eventCode);

      AddEvent(std::move(event));
    } catch (...) {
      std::cerr << "Invalid line in config: " << line << '\n';
      continue;
    }
  }

  bindings.close();
}
