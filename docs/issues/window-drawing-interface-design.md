# Window Drawing Interface Design Decision

## Problem Statement

States need to draw directly (like SFML tutorial), but we need to decide:
- Should `Window` class provide drawing methods (`DrawText()`, `DrawAsset()`, `DrawButton()`)?
- Or should states get function pointers directly from `Drawer`?
- What is the minimal `Window` interface needed for StateManager's `SharedContext`?

## Current Situation

### SFML Tutorial Pattern

**Important: SFML has TWO separate classes:**

1. **`sf::Window`** - Base class for window management
   - Handles window creation, events, focus, fullscreen, etc.
   - Does NOT have drawing capabilities

2. **`sf::RenderWindow`** - Inherits from `sf::Window`, adds rendering
   - Has `draw(sf::Drawable&)` method for rendering
   - Has `clear()`, `display()` for frame management
   - This is the class that actually provides drawing API

**Tutorial's custom Window class:**
- The tutorial creates a custom `Window` class that wraps `sf::RenderWindow`
- It provides `GetRenderWindow()` which returns `sf::RenderWindow*`
- It also provides `Draw(sf::Drawable&)` wrapper, but states don't use this
- It provides `BeginDraw()`, `EndDraw()` for frame management

**How states use it:**
```cpp
void State_MainMenu::Draw() {
    sf::RenderWindow* window = m_stateMgr->GetContext()->
        m_wind->GetRenderWindow();  // Get the RenderWindow pointer
    window->draw(m_text);  // Call RenderWindow's draw() method directly
    window->draw(m_rects[i]);
}
```

**Key observation:** 
- States get the underlying `sf::RenderWindow*` (the rendering object)
- They call `RenderWindow::draw()` directly - the drawing API is in RenderWindow, not Window
- The tutorial's Window class is just a wrapper that provides access to RenderWindow
- States don't use Window's `Draw()` wrapper method - they use RenderWindow's native API

### Network-Snake Current Pattern

**Current Drawer implementation:**
- Drawer loads dynamic library and gets function pointers
- Drawer owns `void* window` pointer
- Drawer has methods: `drawMenu()`, `drawGameField()` - Drawer does the drawing
- Function pointers: `drawText`, `drawAsset`, `drawButton` (from dynamic library)

**Function pointer signatures:**
```cpp
typedef void (*drawTextFunc)(void *, float, float, int, const char *);
typedef void (*drawAssetFunc)(void *, float, float, float, float, const char *);
typedef void (*drawButtonFunc)(void *, float, float, float, float, const char *);
```

## Design Options

### Option A: Window Provides Drawing Methods (Current Proposal)

**Window interface:**
```cpp
class Window {
public:
    void DrawText(float x, float y, int size, const char* text);
    void DrawAsset(float x, float y, float w, float h, const char* asset);
    void DrawButton(float x, float y, float w, float h, const char* label);
    void Close();
    
private:
    void* window;
    drawTextFunc drawText;
    drawAssetFunc drawAsset;
    drawButtonFunc drawButton;
    // ... setup methods for Drawer
};
```

**States use it:**
```cpp
void StateMainMenu::Draw() {
    Window* window = GetContext()->window;
    window->DrawText(380, 200, 40, "42 SNAKES");
    window->DrawButton(x, y, w, h, "MULTIPLAYER");
}
```

**Pros:**
- Clean interface - states don't need to know about function pointers
- Encapsulates window pointer and function pointers
- Matches abstraction level of SFML's Window (provides drawing interface)

**Cons:**
- Window needs to store function pointers (but Drawer already has them)
- Window becomes more than just a window wrapper

### Option B: States Get Function Pointers Directly

**Window interface:**
```cpp
class Window {
public:
    void* GetWindowPtr();  // Like GetRenderWindow() in SFML
    void Close();
};
```

**States use it:**
```cpp
void StateMainMenu::Draw() {
    Window* window = GetContext()->window;
    void* winPtr = window->GetWindowPtr();
    // States need access to function pointers somehow
    drawText(winPtr, 380, 200, 40, "42 SNAKES");
    drawButton(winPtr, x, y, w, h, "MULTIPLAYER");
}
```

**Problem:** States need function pointers. Where do they come from?
- Option B1: SharedContext also has function pointers
- Option B2: States get them from Drawer somehow
- Option B3: Window provides getters for function pointers

**Pros:**
- Window is minimal - just wraps window pointer
- More similar to SFML pattern (GetRenderWindow)

**Cons:**
- States need to manage function pointers themselves
- Less encapsulation
- SharedContext becomes more complex

### Option C: Drawer Provides Drawing Interface

**Window interface:**
```cpp
class Window {
public:
    void Close();
    // Minimal - just window management
};
```

**SharedContext:**
```cpp
struct SharedContext {
    Window* window;
    Drawer* drawer;  // States get drawing from Drawer
    EventManager* eventManager;
};
```

**States use it:**
```cpp
void StateMainMenu::Draw() {
    Drawer* drawer = GetContext()->drawer;
    drawer->DrawText(380, 200, 40, "42 SNAKES");
    drawer->DrawButton(x, y, w, h, "MULTIPLAYER");
}
```

**Pros:**
- Window is truly minimal
- Drawer already has function pointers

**Cons:**
- States depend on Drawer (not just Window)
- SharedContext needs Drawer pointer
- Less separation of concerns (Window vs Drawer)

## Analysis: What Does SFML's Window Actually Provide?

**SFML class hierarchy:**
- `sf::Window` (base) - window management only, no drawing
- `sf::RenderWindow` (derived) - adds `draw()`, `clear()`, `display()` methods

**Tutorial's Window wrapper methods used by states:**
- `GetRenderWindow()` - **This is the key method** - returns `sf::RenderWindow*`
- `Close()` - sometimes used
- `GetWindowSize()` - sometimes used

**Tutorial's Window wrapper methods NOT used by states:**
- `Draw(sf::Drawable&)` - states use `RenderWindow::draw()` directly
- `BeginDraw()`, `EndDraw()` - called by Game/StateManager, not states

**Key insight:** 
- SFML separates window management (`sf::Window`) from rendering (`sf::RenderWindow`)
- The tutorial's Window wrapper provides access to the rendering object (`sf::RenderWindow*`)
- States use `RenderWindow::draw()` - the drawing API is in RenderWindow, not Window
- The tutorial's Window is just a convenience wrapper that gives access to RenderWindow

## Network-Snake Equivalent

**What is the "underlying rendering object" in network-snake?**
- SFML has `sf::RenderWindow` - a single object with `draw()` method
- Network-snake has `void* window` pointer + function pointers (no single object)
- The rendering API IS the function pointers themselves
- There's no equivalent to `sf::RenderWindow` - just function pointers

**So the equivalent of `GetRenderWindow()` would be:**
- Option A: Window provides drawing methods (wraps function pointers) - **Like RenderWindow's draw()**
- Option B: Window provides `GetWindowPtr()` + function pointers (exposes internals) - **Like GetRenderWindow()**

**Key difference:**
- SFML: `Window` → `GetRenderWindow()` → `RenderWindow::draw()`
- Network-snake: `Window` → `DrawText()` (wraps function pointer call)

## Recommendation

**Option A is better because:**

1. **Encapsulation:** Window encapsulates both the window pointer AND the drawing API (function pointers). States don't need to know about function pointers.

2. **Consistency with abstraction:** 
   - SFML: `Window::GetRenderWindow()` → `RenderWindow::draw()` (rendering API in RenderWindow)
   - Network-snake: `Window::DrawText()` (rendering API in Window itself)
   - Both provide a complete drawing interface, just structured differently

3. **Graphics-agnostic states:** States call `window->DrawText()` without knowing it's a function pointer. If we change the graphics library, only Window changes, not states.

4. **Minimal SharedContext:** States only need `Window*`, not `Window*` + function pointers.

5. **Architectural similarity:** 
   - SFML separates Window (management) from RenderWindow (rendering)
   - Network-snake: Window combines both (management + rendering via function pointers)
   - This is acceptable because we don't have a separate "RenderWindow" class - Window IS the rendering interface

**However, Window should be minimal:**
- Public: Drawing methods (`DrawText`, `DrawAsset`, `DrawButton`), `Close()`
- Private/Friend: Setup methods (`Initialize`, `SetFunctions`), frame management (`BeginFrame`, `EndFrame`), event checking (`CheckEvents`)

## Open Questions

1. **Do states need `GetWindowSize()`?** - Check if any states use window size for layout
2. **Do states need `IsOpen()`?** - Probably not, StateManager handles this
3. **Should `Close()` be in Window or handled by StateManager?** - States might want to close window (like MainMenu's Exit button)

## Decision: Window Replaces Both sf::Window and sf::RenderWindow

**Important architectural note:**

In SFML, there are two separate classes:
- `sf::Window` - window management (creation, events, focus, fullscreen)
- `sf::RenderWindow` - rendering capabilities (draw(), clear(), display())

The tutorial's custom `Window` class wraps `sf::RenderWindow`, effectively providing both window management AND rendering in a single class.

**Our Window class should do the same:**
- Replace `sf::Window` functionality: window management (Close(), window pointer management)
- Replace `sf::RenderWindow` functionality: rendering (DrawText(), DrawAsset(), DrawButton())

**Why combine them:**
1. We don't have separate classes - no "RenderWindow" equivalent
2. The rendering API is function pointers, not a separate class
3. Window already owns the `void* window` pointer - it should also own the function pointers
4. States access Window through SharedContext - it should provide the complete graphics interface
5. Consistency with tutorial's pattern - their Window wraps RenderWindow, providing both

**Conclusion:** Window is the complete graphics interface (management + rendering), similar to how the tutorial's Window wraps RenderWindow.

## Focus Handling

**Window should handle focus events:**

The Window class should track and handle window focus state, similar to the SFML tutorial's Window class.

**Why focus handling is important:**
- Prevents the game from processing input when the user switches to another application
- Avoids keys being registered when typing in another window
- Prevents game logic from running when the window is in the background
- Prevents unintended actions when the window is not active

**Implementation pattern (from SFML tutorial):**

1. **Window tracks focus state:**
   - Handle `FOCUS_LOST` and `FOCUS_GAINED` events from graphics library
   - Maintain internal `m_isFocused` boolean flag
   - Provide `bool IsFocused()` method for querying focus state

2. **Window notifies EventManager:**
   - When `FOCUS_LOST` event occurs: call `EventManager::SetFocus(false)`
   - When `FOCUS_GAINED` event occurs: call `EventManager::SetFocus(true)`
   - Window needs access to EventManager (via SharedContext or constructor parameter)

3. **EventManager skips processing when unfocused:**
   - `EventManager::Update()` should check focus state:
     ```cpp
     void EventManager::Update() {
         if (!m_hasFocus) {
             return;  // Don't process events if window doesn't have focus
         }
         // ... process bindings and callbacks
     }
     ```

**Window interface additions:**
- `bool IsFocused()` - public method to check if window has focus
- Internal: Handle `FOCUS_LOST` and `FOCUS_GAINED` events in event polling
- Internal: Call `EventManager::SetFocus()` when focus changes

**EventManager interface additions:**
- `void SetFocus(bool hasFocus)` - method to set focus state
- Internal: `bool m_hasFocus` member variable (default: `true`)
- `Update()` method should check `m_hasFocus` before processing

**Note:** This matches the SFML tutorial pattern where Window handles focus events and EventManager respects focus state to prevent unwanted input processing.

## Next Steps

1. Check if states need window size
2. Decide on `Close()` placement
3. Implement minimal Window with drawing methods
4. Update SharedContext to use `Window*` instead of `Drawer*`
5. Add comment in Window class explaining it replaces both sf::Window and sf::RenderWindow

