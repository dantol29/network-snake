#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include "../includes/nibbler.hpp"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

enum class EventType {
  Closed,
  Resized,
  FocusLost,
  FocusGained,
  TextEntered,
  KeyPressed,
  KeyReleased,
  MouseWheelScrolled,
  MouseButtonPressed,
  MouseButtonReleased,
  MouseMoved,
  MouseMovedRaw,
  MouseEntered,
  MouseLeft,
  Keyboard,
  Mouse,
  Joystick
};

using EventCode = int;
using Events = std::vector<std::pair<EventType, EventCode>>;
using EventName = std::string;
using CallbackContainer = std::unordered_map<std::string, std::function<void(t_event*)>>;
using Callbacks = std::unordered_map<StateType, CallbackContainer>;

struct EventState {
  EventState(const std::string& name) : name_(name), matchedEvents_(0) {}

  void AddEvent(EventType type, EventCode code) { events_.emplace_back(type, code); }

  Events events_;
  std::string name_;
  size_t matchedEvents_;
};

using EventStates = std::unordered_map<EventName, std::unique_ptr<EventState>>;

class EventManager {
public:
  EventManager();
  ~EventManager() = default;

  EventManager(const EventManager&) = delete;
  EventManager& operator=(const EventManager&) = delete;
  EventManager(EventManager&&) = default;
  EventManager& operator=(EventManager&&) = default;

  bool AddEvent(std::unique_ptr<EventState> event);
  bool RemoveEvent(std::string name);

  void SetFocus(const bool& has_focus);
  void SetCurrentState(StateType state);

  // NOTE: The 'name' parameter must match the name defined in keys.cfg for the
  // desired triggering configuration
  template <class T>
  bool AddCallback(StateType state, const std::string& name, void (T::*func)(t_event*),
                   T* instance) {
    auto it = callbacks_.emplace(state, CallbackContainer()).first;
    auto temp = std::bind(func, instance, std::placeholders::_1);
    return it->second.emplace(name, temp).second;
  }

  bool RemoveCallback(StateType state, const std::string& name);

  void HandleEvent(t_event& event);
  void Update();

private:
  void LoadTargetEventBindings();

  t_event lastEvent_;
  EventStates eventStates_;
  Callbacks callbacks_;
  bool has_focus_;
  StateType current_state_;
};

#endif