#ifndef SDLWINDOW_H
#define SDLWINDOW_H

#include <SDL.h>

#include <exception>
#include <string>

class SDLWindow {
   public:
    SDL_Window *m_sdl_window{};
    SDLWindow() = default;
    SDLWindow(SDLWindow const &) = delete;
    SDLWindow(SDLWindow &&) noexcept;
    void init(const char *title, int x, int y, int w, int h, Uint32 flags);
    ~SDLWindow();
};

#endif  // SDLWINDOW_H
