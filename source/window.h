#ifndef WINDOW_H
#define WINDOW_H

#include <SDL.h>
#include <glad/glad.h>

#include <exception>
#include <string>

#include "input.h"
#include "sdlglcontext.h"
#include "sdlwindow.h"

class Window {
   public:
    friend void Input::set_mouse_position(Window& window, int x,
                                          int y) noexcept;

    int m_width{};
    int m_height{};
    bool m_window_resized{};
    uint32_t m_last_frame_time{};
    uint32_t m_this_frame_time{};

    Window(int width, int height, std::string const& title,
           bool resizable = false, bool maximize = false,
           bool fullscreen = false);

    Window(Window const&) = delete;

    void set_title(char const* buffer);
    void tick() noexcept;
    void swap_buffers() noexcept;

    ~Window() = default;

   private:
    bool resizable_{};
    SDLWindow sdl_window_{};
    SDLGLContext sdl_gl_context_{};
};

#endif  // WINDOW_H
