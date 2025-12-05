#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "../includes/nibbler.hpp"

/// @brief Window class that replaces both sf::Window and sf::RenderWindow
///
/// In SFML, there are two separate classes:
/// - sf::Window: window management (creation, events, focus, fullscreen)
/// - sf::RenderWindow: rendering capabilities (draw(), clear(), display())
///
/// The tutorial's custom Window class wraps sf::RenderWindow, providing both
/// window management AND rendering in a single class.
///
/// Our Window class follows the same pattern:
/// - Replaces sf::Window: window management (Close(), window pointer management)
/// - Replaces sf::RenderWindow: rendering (DrawText(), DrawAsset(), DrawButton())
///
/// This design makes sense because:
/// - We don't have separate classes (no "RenderWindow" equivalent)
/// - The rendering API is function pointers, not a separate class
/// - Window owns the void* window pointer, so it should also own the function pointers
/// - States access Window through SharedContext - it provides the complete graphics interface
///
/// Window is the complete graphics interface (management + rendering), similar to
/// how the tutorial's Window wraps RenderWindow.
class Window {
public:
    Window();
    ~Window();

    // Rule of Five: Copy operations deleted (window pointer is not copyable)
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    // Move operations deleted (window pointer is not moveable in a useful way)
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    // Initialize window with function pointers and create window
    void Initialize(initFunc init, int width, int height, void* userData);
    
    // Set function pointers (called after loading dynamic library)
    void SetFunctions(
        checkEventsFunc checkEvents,
        beginFrameFunc beginFrame,
        endFrameFunc endFrame,
        drawTextFunc drawText,
        drawAssetFunc drawAsset,
        drawButtonFunc drawButton,
        cleanupFunc cleanup
    );

    // Window operations
    void* GetWindowPtr() const;
    bool IsOpen() const;
    void Close();

    // Frame management
    void BeginFrame();
    void EndFrame();

    // Event checking
    t_event CheckEvents();

    // Drawing operations
    void DrawText(float x, float y, int size, const char* text);
    void DrawAsset(float x, float y, float width, float height, const char* asset);
    void DrawButton(float x, float y, float width, float height, const char* label);

private:
    void* window;
    
    // Function pointers
    initFunc init;
    checkEventsFunc checkEvents;
    beginFrameFunc beginFrame;
    endFrameFunc endFrame;
    drawTextFunc drawText;
    drawAssetFunc drawAsset;
    drawButtonFunc drawButton;
    cleanupFunc cleanup;
};

#endif
