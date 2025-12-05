#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include "../includes/nibbler.hpp" // For Vec2i
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// StateType enum for state-aware callbacks
// NOTE: This is defined here (not in StateManager) to keep EventManager
// independent. State-aware callbacks allow different callbacks to be registered
// for different UI states (e.g., menu vs game). If StateManager is added later,
// this can be moved there.
enum class StateType {
  Global = 0, // Global callbacks (always active regardless of state)
  Menu = 1,
  Game = 2,
  GameOver = 3,
  Paused = 4
};

// SFML 3.x uses variant-based events, so we use numeric values for EventType
// These correspond to the variant index positions in sf::Event
enum class EventType {
  Closed = 0,
  Resized = 1,
  FocusLost = 2,
  FocusGained = 3,
  TextEntered = 4,
  KeyPressed = 5,
  KeyReleased = 6,
  MouseWheelScrolled = 7,
  MouseButtonPressed = 8,
  MouseButtonReleased = 9,
  MouseMoved = 10,
  MouseMovedRaw = 11,
  MouseEntered = 12,
  MouseLeft = 13,
  // Custom categories (after all SFML events)
  Keyboard = 20,
  Mouse = 21,
  Joystick = 22
};

struct EventInfo {
  EventInfo() { m_code = 0; }
  EventInfo(int l_event) { m_code = l_event; }

  union {
    int m_code;
  };
};

using Events = std::vector<std::pair<EventType, EventInfo>>;

struct EventDetails {
  EventDetails(const std::string &l_bindName) : m_name(l_bindName) { Clear(); }

  std::string m_name;
  // NOTE: m_size naming is vague - doesn't indicate what size or where it comes
  // from. Better names would be: windowWidth/windowHeight, or
  // resizedWidth/resizedHeight. Functionally, it's only used for
  // sf::Event::Resized, so context limits confusion.
  Vec2i m_size;
  std::uint32_t m_textEntered;
  Vec2i m_mouse;
  int m_mouseWheelDelta;
  int m_keyCode; // Single key code.

  void Clear() {
    m_size = Vec2i{0, 0};
    m_textEntered = 0;
    m_mouse = Vec2i{0, 0};
    m_mouseWheelDelta = 0;
    m_keyCode = -1;
  }
};

struct Binding {
  Binding(const std::string &l_name)
      : m_name(l_name), c(0), m_details(l_name) {}

  // NOTE: Consider adding a method that takes an array/vector of event pairs
  // to bind multiple events at once, rather than calling BindEvent multiple
  // times. Alternatively, consider an EventInfo constructor that takes an array
  // of events.
  void BindEvent(EventType l_type, EventInfo l_info = EventInfo()) {
    m_events.emplace_back(l_type, l_info);
  }

  Events m_events;
  std::string m_name;
  int c; // Count of events that are "happening".
  EventDetails m_details;
};

using Bindings = std::unordered_map<std::string, Binding *>;
// State-aware callback types
using CallbackContainer =
    std::unordered_map<std::string, std::function<void(EventDetails *)>>;
using Callbacks = std::unordered_map<StateType, CallbackContainer>;

class EventManager {
public:
  EventManager();
  ~EventManager();

  bool AddBinding(Binding *l_binding);
  bool RemoveBinding(std::string l_name);

  void SetFocus(const bool &l_focus);
  void SetCurrentState(StateType l_state);

  // Needs to be defined in the header!
  template <class T>
  bool AddCallback(StateType l_state, const std::string &l_name,
                   void (T::*l_func)(EventDetails *), T *l_instance) {
    auto itr = m_callbacks.emplace(l_state, CallbackContainer()).first;
    auto temp = std::bind(l_func, l_instance, std::placeholders::_1);
    return itr->second.emplace(l_name, temp).second;
  }

  bool RemoveCallback(StateType l_state, const std::string &l_name);

  void HandleEvent(t_event &l_event);
  void Update();

private:
  void LoadBindings();

  Bindings m_bindings;
  Callbacks m_callbacks;
  bool m_hasFocus;
  StateType m_currentState;
};

// SFML types used in this file:
// - sf::Event: Event type from SFML, used as base values for EventType enum

#endif