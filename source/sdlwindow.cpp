#include "sdlwindow.h"

#include <stdexcept>

void SDLWindow::init(const char* title, int x, int y, int w, int h,
                     Uint32 flags) {
    m_sdl_window = SDL_CreateWindow(title, x, y, w, h, flags);

    if (m_sdl_window == nullptr) {
        throw std::runtime_error(std::string{SDL_GetError()});
    }
}

SDLWindow::SDLWindow(SDLWindow&& win) noexcept
    : m_sdl_window{win.m_sdl_window} {
    win.m_sdl_window = nullptr;
}

SDLWindow::~SDLWindow() { SDL_DestroyWindow(m_sdl_window); }
