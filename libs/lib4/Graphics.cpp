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
        SDL_Surface* surface = IMG_Load(paths[i]);
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

void Graphics::drawAsset(float pixelX, float pixelY, float pixelWidth, float pixelHeight, const char *assetPath)
{
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
