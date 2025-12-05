#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include "../includes/nibbler.hpp" // For Vec2i
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

enum class StateType { Global, Menu, Game, GameOver, Paused };

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
  Binding(const std::string &name)
      : name_(name), matched_count_(0), details_(name) {}

  // NOTE: Consider adding a method that takes an array/vector of event pairs
  // to bind multiple events at once, rather than calling BindEvent multiple
  // times. Alternatively, consider an EventInfo constructor that takes an array
  // of events.
  void BindEvent(EventType type, EventInfo info = EventInfo()) {
    events_.emplace_back(type, info);
  }

  Events events_;
  std::string name_;
  int matched_count_; // Count of events that have matched so far.
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