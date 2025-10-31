#include "Graphics.hpp"
#include <iostream>
#include <optional>

Drawer *drawer = nullptr;

Graphics::Graphics(unsigned int height, unsigned int width, void *gamePointer)
{
    drawer = static_cast<Drawer *>(gamePointer);
    windowWidth = static_cast<float>(width);
    windowHeight = static_cast<float>(height);

    if (!SDL_Init(SDL_INIT_VIDEO))
        throw std::runtime_error(SDL_GetError());

    if (!TTF_Init())
        throw std::runtime_error(SDL_GetError());

    gameWindow = SDL_CreateWindow("SDL3 Window", width, height, SDL_WINDOW_RESIZABLE);
    if (!gameWindow)
        throw std::runtime_error(SDL_GetError());

    SDL_SetWindowPosition(gameWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    renderer = SDL_CreateRenderer(gameWindow, NULL);
    if (!renderer)
        throw std::runtime_error(SDL_GetError());

    font = TTF_OpenFont("assets/Montserrat-Bold.ttf", 24);
    if (!font)
        std::cerr << "Could not open font: " << SDL_GetError() << std::endl;
}

Graphics::~Graphics()
{
    std::cout << "Destructor SDL " << std::endl;
    
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (gameWindow) SDL_DestroyWindow(gameWindow);
    TTF_Quit();
    SDL_Quit();

}

void Graphics::closeWindow()
{
    if (gameWindow)
        SDL_DestroyWindow(gameWindow);
    gameWindow = nullptr;
}

void Graphics::loop()
{
    SDL_Event event;

    while (gameWindow != nullptr)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    this->closeWindow();
                    break;

                case SDL_EVENT_KEY_DOWN:
                    keyCallback(event.key);
                    break;

                case SDL_EVENT_MOUSE_BUTTON_UP:
                    onMouseUp(event.button);
                    break;
            }
        }
        drawer->onEachFrame(false);
    }
}

void Graphics::drawSquare(float pixelX, float pixelY, float pixelWidth, float pixelHeight, struct rgb color)
{
    SDL_FRect rect;
    rect.x = pixelX;
    rect.y = pixelY;
    rect.w = pixelWidth;
    rect.h = pixelHeight;

    SDL_SetRenderDrawColor(renderer,
                           static_cast<Uint8>(color.r * 255),
                           static_cast<Uint8>(color.g * 255),
                           static_cast<Uint8>(color.b * 255),
                           255);
    SDL_RenderFillRect(renderer, &rect);
}

void Graphics::drawText(float x, float y, int size, const char *text)
{
    (void)size;
    if (!font || !renderer || !text) return;

    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, SDL_strlen(text), color);
    if (!surface) {
        SDL_Log("TTF_RenderText_Blended failed: %s", SDL_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        SDL_DestroySurface(surface);
        return;
    }

    SDL_FRect dstRect = {x, y, static_cast<float>(surface->w), static_cast<float>(surface->h)};

    SDL_RenderTexture(renderer, texture, nullptr, &dstRect);

    SDL_DestroySurface(surface);
    SDL_DestroyTexture(texture);
}

void Graphics::drawButton(float x, float y, float width, float height, const char *text)
{
    SDL_FRect rect = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 120, 120, 100, 255);
    SDL_RenderFillRect(renderer, &rect);

    if (!font || !text) return;

    SDL_Color color = {255, 255, 255, 255}; // White
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, SDL_strlen(text), color);
    if (!surface) {
        SDL_Log("TTF_RenderText_Blended failed: %s", SDL_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        SDL_DestroySurface(surface);
        return;
    }

    SDL_FRect dstRect;
    dstRect.w = static_cast<float>(surface->w);
    dstRect.h = static_cast<float>(surface->h);
    dstRect.x = x + (width - dstRect.w) / 2.0f;
    dstRect.y = y + (height - dstRect.h) / 2.0f;

    SDL_RenderTexture(renderer, texture, nullptr, &dstRect);

    SDL_DestroySurface(surface);
    SDL_DestroyTexture(texture);
}

void Graphics::display()
{
    SDL_RenderPresent(renderer);
}

void Graphics::cleanScreen()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Graphics::onMouseUp(const SDL_MouseButtonEvent &buttonEvent)
{
    if (buttonEvent.button == SDL_BUTTON_LEFT)
        drawer->onMouseUp(buttonEvent.x, buttonEvent.y);
}

void Graphics::keyCallback(const SDL_KeyboardEvent &keyEvent)
{
    SDL_Keycode code = keyEvent.key;

    switch (code)
    {
    case SDLK_W:
    case SDLK_UP:
        drawer->keyCallback(UP, 1);
        break;
    case SDLK_S:
    case SDLK_DOWN:
        drawer->keyCallback(DOWN, 1);
        break;
    case SDLK_A:
    case SDLK_LEFT:
        drawer->keyCallback(LEFT, 1);
        break;
    case SDLK_D:
    case SDLK_RIGHT:
        drawer->keyCallback(RIGHT, 1);
        break;
    case SDLK_M:
        drawer->keyCallback(M, 1);
        break;
    case SDLK_N:
        drawer->keyCallback(N, 1);
        break;
    case SDLK_1:
        drawer->keyCallback(KEY_1, 1);
        break;
    case SDLK_2:
        drawer->keyCallback(KEY_2, 1);
        break;
    case SDLK_3:
        drawer->keyCallback(KEY_3, 1);
        break;
    default:
        break;
    }
}

