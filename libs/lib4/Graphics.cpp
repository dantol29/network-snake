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

    if (font)
        TTF_CloseFont(font);
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (gameWindow)
        SDL_DestroyWindow(gameWindow);
    TTF_Quit();
    SDL_Quit();
}

void Graphics::loadAssets(const char **paths)
{
    if (!paths)
        return;

    for (int i = 0; paths[i]; ++i)
    {
        SDL_Surface *surface = IMG_Load(paths[i]);
        if (!surface)
        {
            std::cerr << "Failed to load image: " << paths[i] << " | "
                      << SDL_GetError() << std::endl;
            continue;
        }

        SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);

        if (!tex)
        {
            std::cerr << "Failed to create texture: " << paths[i] << " | "
                      << SDL_GetError() << std::endl;
            continue;
        }

        // Store in assets map
        assets.insert({std::string(paths[i]), tex});
    }
}

void Graphics::drawAsset(float pixelX, float pixelY, float pixelWidth, float pixelHeight, int degrees, const char *assetPath)
{
    (void)degrees; // TODO: Implement rotation support
    try
    {
        SDL_Texture *tex = assets.at(assetPath);

        SDL_FRect dest;
        dest.x = pixelX;
        dest.y = pixelY;
        dest.w = pixelWidth;
        dest.h = pixelHeight;

        if (!SDL_RenderTexture(renderer, tex, nullptr, &dest))
        {
            std::cerr << "SDL_RenderTexture error: " << SDL_GetError() << std::endl;
        };
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Key not found!\n"
                  << std::endl;
    }
}

void Graphics::drawText(float x, float y, int size, const char *text)
{
    (void)size;
    if (!font || !renderer || !text)
        return;

    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, SDL_strlen(text), color);
    if (!surface)
    {
        SDL_Log("TTF_RenderText_Blended failed: %s", SDL_GetError());
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
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

    if (!font || !text)
        return;

    SDL_Color color = {255, 255, 255, 255}; // White
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, SDL_strlen(text), color);
    if (!surface)
    {
        SDL_Log("TTF_RenderText_Blended failed: %s", SDL_GetError());
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
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
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Graphics::endFrame()
{
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
            e.type = CLOSED;
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
        event.mouse.x = buttonEvent.x;
        event.mouse.y = buttonEvent.y;
        event.mouse.button = 0; // Left button = 0
        event.type = MOUSE_BUTTON_RELEASED;
    }

    return event;
}

t_event Graphics::onKeyPress(const SDL_KeyboardEvent &keyEvent)
{
    t_event event;
    event.type = KEY_PRESSED;

    switch (keyEvent.key)
    {
    case SDLK_W:
        event.keyCode = 22; // SFML W
        break;
    case SDLK_UP:
        event.keyCode = 73; // SFML Up
        break;
    case SDLK_S:
        event.keyCode = 18; // SFML S
        break;
    case SDLK_DOWN:
        event.keyCode = 74; // SFML Down
        break;
    case SDLK_A:
        event.keyCode = 0; // SFML A
        break;
    case SDLK_LEFT:
        event.keyCode = 71; // SFML Left
        break;
    case SDLK_D:
        event.keyCode = 3; // SFML D
        break;
    case SDLK_RIGHT:
        event.keyCode = 72; // SFML Right
        break;
    case SDLK_M:
        event.keyCode = 12; // SFML M (approximate)
        break;
    case SDLK_N:
        event.keyCode = 13; // SFML N (approximate)
        break;
    case SDLK_1:
        event.keyCode = 27; // SFML Num1 (approximate)
        break;
    case SDLK_2:
        event.keyCode = 28; // SFML Num2 (approximate)
        break;
    case SDLK_3:
        event.keyCode = 29; // SFML Num3 (approximate)
        break;
    default:
        event.type = EMPTY;
        break;
    }

    return event;
}
