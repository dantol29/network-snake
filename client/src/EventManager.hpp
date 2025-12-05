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
  EventInfo() { code_ = 0; }
  EventInfo(int event) { code_ = event; }

  union {
    int code_;
  };
};

using Events = std::vector<std::pair<EventType, EventInfo>>;

struct EventDetails {
  EventDetails(const std::string &bind_name) : name_(bind_name) { Clear(); }

  std::string name_;
  // NOTE: size_ naming is vague - doesn't indicate what size or where it comes
  // from. Better names would be: windowWidth/windowHeight, or
  // resizedWidth/resizedHeight. Functionally, it's only used for
  // sf::Event::Resized, so context limits confusion.
  Vec2i size_;
  std::uint32_t text_entered_;
  Vec2i mouse_;
  int mouse_wheel_delta_;
  int key_code_; // Single key code.

  void Clear() {
    size_ = Vec2i{0, 0};
    text_entered_ = 0;
    mouse_ = Vec2i{0, 0};
    mouse_wheel_delta_ = 0;
    key_code_ = -1;
  }
};

struct Binding {
  Binding(const std::string &name) : name_(name), c(0), details_(name) {}

  // NOTE: Consider adding a method that takes an array/vector of event pairs
  // to bind multiple events at once, rather than calling BindEvent multiple
  // times. Alternatively, consider an EventInfo constructor that takes an array
  // of events.
  void BindEvent(EventType type, EventInfo info = EventInfo()) {
    events_.emplace_back(type, info);
  }

  Events events_;
  std::string name_;
  int c; // Count of events that are "happening".
  EventDetails details_;
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

  bool AddBinding(Binding *binding);
  bool RemoveBinding(std::string name);

  void SetFocus(const bool &has_focus);
  void SetCurrentState(StateType state);

  // Needs to be defined in the header!
  template <class T>
  bool AddCallback(StateType state, const std::string &name,
                   void (T::*func)(EventDetails *), T *instance) {
    auto it = callbacks_.emplace(state, CallbackContainer()).first;
    auto temp = std::bind(func, instance, std::placeholders::_1);
    return it->second.emplace(name, temp).second;
  }

  bool RemoveCallback(StateType state, const std::string &name);

  void HandleEvent(t_event &event);
  void Update();

private:
  void LoadBindings();

  Bindings bindings_;
  Callbacks callbacks_;
  bool has_focus_;
  StateType current_state_;
};

// SFML types used in this file:
// - sf::Event: Event type from SFML, used as base values for EventType enum

#endif