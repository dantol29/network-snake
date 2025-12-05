#include "Window.hpp"

Window::Window() : window(nullptr), init(nullptr), checkEvents(nullptr),
                   beginFrame(nullptr), endFrame(nullptr), drawText(nullptr),
                   drawAsset(nullptr), drawButton(nullptr), cleanup(nullptr) {
}

Window::~Window() {
    if (window && cleanup) {
        cleanup(window);
        window = nullptr;
    }
}

void Window::Initialize(initFunc initFunc, int width, int height, void* userData) {
    if (!initFunc) {
        throw "Init function pointer is null";
    }
    
    init = initFunc;
    window = init(width, height, userData);
    
    if (!window) {
        throw "Failed to initialize window";
    }
}

void Window::SetFunctions(
    checkEventsFunc checkEventsFunc,
    beginFrameFunc beginFrameFunc,
    endFrameFunc endFrameFunc,
    drawTextFunc drawTextFunc,
    drawAssetFunc drawAssetFunc,
    drawButtonFunc drawButtonFunc,
    cleanupFunc cleanupFunc
) {
    checkEvents = checkEventsFunc;
    beginFrame = beginFrameFunc;
    endFrame = endFrameFunc;
    drawText = drawTextFunc;
    drawAsset = drawAssetFunc;
    drawButton = drawButtonFunc;
    cleanup = cleanupFunc;
}

void* Window::GetWindowPtr() const {
    return window;
}

bool Window::IsOpen() const {
    return window != nullptr;
}

void Window::Close() {
    if (window && cleanup) {
        cleanup(window);
        window = nullptr;
    }
}

void Window::BeginFrame() {
    if (window && beginFrame) {
        beginFrame(window);
    }
}

void Window::EndFrame() {
    if (window && endFrame) {
        endFrame(window);
    }
}

t_event Window::CheckEvents() {
    if (window && checkEvents) {
        return checkEvents(window);
    }
    return t_event{0, 0, 0, 0};
}

void Window::DrawText(float x, float y, int size, const char* text) {
    if (window && drawText) {
        drawText(window, x, y, size, text);
    }
}

void Window::DrawAsset(float x, float y, float width, float height, const char* asset) {
    if (window && drawAsset) {
        drawAsset(window, x, y, width, height, asset);
    }
}

void Window::DrawButton(float x, float y, float width, float height, const char* label) {
    if (window && drawButton) {
        drawButton(window, x, y, width, height, label);
    }
}

