#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include "../includes/nibbler.hpp"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

enum class StateType { Global, Menu, Game, GameOver, Paused };

enum class TargetEventType {
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

struct TargetEventCode {
  TargetEventCode() { code_ = 0; }
  TargetEventCode(int event) { code_ = event; }

  union {
    int code_;
  };
};

using TargetEvents = std::vector<std::pair<TargetEventType, TargetEventCode>>;

struct MatchedEventDetails {
  MatchedEventDetails() { Clear(); }

  Vec2i window_size_;
  std::uint32_t text_entered_;
  Vec2i mouse_position_;
  int mouse_wheel_delta_;
  int key_code_;

  void Clear() {
    window_size_ = Vec2i{0, 0};
    text_entered_ = 0;
    mouse_position_ = Vec2i{0, 0};
    mouse_wheel_delta_ = 0;
    key_code_ = -1;
  }
};

struct TargetEventBindingState {
  TargetEventBindingState(const std::string &name)
      : name_(name), matched_count_(0), details_() {}

  void AddTargetEvent(TargetEventType type,
                      TargetEventCode code = TargetEventCode()) {
    events_.emplace_back(type, code);
  }

  TargetEvents events_;
  std::string name_;
  int matched_count_;
  MatchedEventDetails details_;
};

using TargetEventBindingStates =
    std::unordered_map<std::string, std::unique_ptr<TargetEventBindingState>>;
using CallbackContainer =
    std::unordered_map<std::string, std::function<void(MatchedEventDetails *)>>;
using Callbacks = std::unordered_map<StateType, CallbackContainer>;

class EventManager {
public:
  EventManager();

  bool AddBinding(std::unique_ptr<TargetEventBindingState> binding);
  bool RemoveBinding(std::string name);

  void SetFocus(const bool &has_focus);
  void SetCurrentState(StateType state);

  template <class T>
  bool AddCallback(StateType state, const std::string &name,
                   void (T::*func)(MatchedEventDetails *), T *instance) {
    auto it = callbacks_.emplace(state, CallbackContainer()).first;
    auto temp = std::bind(func, instance, std::placeholders::_1);
    return it->second.emplace(name, temp).second;
  }

  bool RemoveCallback(StateType state, const std::string &name);

  void HandleEvent(t_event &event);
  void Update();

private:
  void LoadTargetEventBindings();

  TargetEventBindingStates bindings_;
  Callbacks callbacks_;
  bool has_focus_;
  StateType current_state_;
};

#endif