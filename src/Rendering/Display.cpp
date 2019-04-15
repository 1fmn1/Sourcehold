#include <Rendering/Display.h>

using namespace Sourcehold::System;
using namespace Sourcehold::Rendering;

Display::Display() :
    Renderer()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        Logger::error("RENDERING") << SDL_GetError() << std::endl;
    }
}

Display::~Display() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Display::Open(const std::string &title, int width, int height, int index, bool fullscreen, bool noborder, bool nograb) {
    int param = SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS;

    this->fullscreen = fullscreen;
    if(fullscreen) {
        param |= SDL_WINDOW_FULLSCREEN;
    }

    if(noborder) {
        param |= SDL_WINDOW_BORDERLESS;
    }

    /* Select display */
    int displays = SDL_GetNumVideoDisplays();
    if(index >= displays) index = 0;

    SDL_Rect *displayBounds = new SDL_Rect[displays];
    for(int i = 0; i < displays; i++) {
        displayBounds[i] = { 0, 0, 0, 0 };
        SDL_GetDisplayBounds(i, &displayBounds[i]);
    }

    window = SDL_CreateWindow(
        title.c_str(),
        displayBounds[index].x + (displayBounds[index].w / 2) - width / 2,
        displayBounds[index].y + (displayBounds[index].h / 2) - height / 2,
        width,
        height,
        param
    );
    if(!window) {
        Logger::error("GAME") << "Unable to create SDL2 window: " << SDL_GetError() << std::endl;
    }

    delete [] displayBounds;

    if(!nograb) {
        SDL_SetWindowGrab(window, SDL_TRUE);
    }

    Renderer::Init(window);

    open = true;
}

void Display::ToggleFullscreen() {
    fullscreen = !fullscreen;

    int err = SDL_SetWindowFullscreen(window, fullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
    if(err < 0) {
        Logger::error("GAME")  << "Unable to switch to fullscreen: " << SDL_GetError() << std::endl;
    }
}

void Display::StartTimer() {
    timer = SDL_GetTicks();
}

void Display::EndTimer() {
    int32_t ntimer = SDL_GetTicks();
    int32_t delta = ntimer - timer;

    if(delta < 0) return;

    timer = ntimer;
    if(delta < 1000 / FRAMES_PER_SECOND) {
        SDL_Delay((1000 / FRAMES_PER_SECOND) - delta);
    }
}

bool Display::IsOpen() {
    return open;
}

uint32_t Display::GetTicks() {
    return SDL_GetTicks();
}

uint32_t Display::GetSeconds() {
    return SDL_GetTicks() / 1000;
}
