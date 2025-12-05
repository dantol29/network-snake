#pragma once

#include "BaseState.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <functional>
#include <string>
#include <type_traits>

// Forward declarations
class Window;
class EventManager;

// State type enumeration
enum class StateType {
    Intro = 1,
    MainMenu,
    Game,
    Mushroom,
    Paused,
    GameOver,
    Credits
};

/// @brief Shared context structure providing access to common resources for all states.
///
/// This structure holds pointers to resources that are shared across all game states.
/// Instead of each state maintaining its own copies of Window and EventManager, they
/// all access the same instances through this shared context.
///
/// The SharedContext is:
/// - Created once in the Game class
/// - Passed to the StateManager constructor
/// - Accessed by states through m_stateMgr->GetContext()
///
/// @note This structure can be extended later to include additional shared resources
///       such as player data, resource managers, sound managers, or networking components.
///
/// @example
/// @code
/// // In any state, access shared resources:
/// void StateIntro::OnCreate() {
///     sf::Vector2u windowSize = m_stateMgr->GetContext()->
///         m_wind->GetRenderWindow()->getSize();
///     
///     EventManager* evMgr = m_stateMgr->GetContext()->m_eventManager;
///     evMgr->AddCallback(StateType::Intro, "Intro_Continue",
///         &StateIntro::Continue, this);
/// }
/// @endcode
struct SharedContext {
    /// @brief Default constructor initializes all pointers to nullptr.
    SharedContext() : m_wind(nullptr), m_eventManager(nullptr) {}
    
    /// @brief Pointer to the main window instance (shared across all states).
    Window* m_wind;
    
    /// @brief Pointer to the event manager instance (shared across all states).
    EventManager* m_eventManager;
};

// Type definitions
using StateContainer = std::vector<std::pair<StateType, BaseState*>>;
using TypeContainer = std::vector<StateType>;
using StateFactory = std::unordered_map<StateType, std::function<BaseState*(void)>>;

class StateManager {
public:
    StateManager(SharedContext* l_shared);
    ~StateManager();
    
    void Update(const sf::Time& l_time);
    void Draw();
    void ProcessRequests();
    
    SharedContext* GetContext();
    bool HasState(const StateType& l_type);
    void SwitchTo(const StateType& l_type);
    void Remove(const StateType& l_type);
    
    template<class T>
    void RegisterState(const StateType& l_type);

private:
    void CreateState(const StateType& l_type);
    void RemoveState(const StateType& l_type);
    
    SharedContext* m_shared;
    StateContainer m_states;
    TypeContainer m_toRemove;
    StateFactory m_stateFactory;
};

// Template implementation
// NOTE: Template methods must be in the header file for proper instantiation.
// Type safety: static_assert ensures T derives from BaseState at compile time.
template<class T>
void StateManager::RegisterState(const StateType& l_type) {
    static_assert(std::is_base_of_v<BaseState, T>,
                  "T must derive from BaseState");
    m_stateFactory[l_type] = [this]() -> BaseState* {
        return new T(this);
    };
}

