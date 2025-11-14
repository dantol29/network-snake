#include "Graphics.hpp"
#include <iostream>
#include <optional>

Graphics::Graphics(unsigned int height, unsigned int width) : IGraphics(height, width)
{
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

void Graphics::beginFrame() 
{
    if (this->shouldUpdateScreen) 
    {
        this->shouldUpdateScreen = false;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }
}

void Graphics::endFrame() 
{
    if (this->shouldUpdateScreen)
        SDL_RenderPresent(renderer);
}

t_event Graphics::checkEvents() 
{
    t_event e;
    e.type = EMPTY;

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
                e.type = EXIT;
                return e;

            case SDL_EVENT_KEY_DOWN:
                return onKeyPress(event.key);

            case SDL_EVENT_MOUSE_BUTTON_UP:
                return onMouseUp(event.button);
        }
    }

    return e;
}

t_event Graphics::onMouseUp(const SDL_MouseButtonEvent &buttonEvent)
{
    t_event event;
    event.type = EMPTY;

    if (buttonEvent.button == SDL_BUTTON_LEFT) 
    {
        event.a = buttonEvent.x;
        event.b = buttonEvent.y;
        event.type = MOUSE;
    }

    return event;
}

t_event Graphics::onKeyPress(const SDL_KeyboardEvent &keyEvent)
{
    t_event event;
    event.type = KEY;

    SDL_Keycode code = keyEvent.key;

    switch (code)
    {
    case SDLK_W:
    case SDLK_UP:
        event.a = UP;
        break;
    case SDLK_S:
    case SDLK_DOWN:
        event.a = DOWN;
        break;
    case SDLK_A:
    case SDLK_LEFT:
        event.a = LEFT;
        break;
    case SDLK_D:
    case SDLK_RIGHT:
        event.a = RIGHT;
        break;
    case SDLK_M:
        event.a = M;
        break;
    case SDLK_N:
        event.a = N;
        break;
    case SDLK_1:
        event.a = KEY_1;
        break;
    case SDLK_2:
        event.a = KEY_2;
        break;
    case SDLK_3:
        event.a = KEY_3;
        break;
    default:
        event.type = EMPTY;
        break;
    }

    return event;
}

