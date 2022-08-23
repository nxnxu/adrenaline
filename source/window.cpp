#include "window.h"

#include <stdexcept>

Window::Window(int width, int height, std::string const& title, bool resizable,
               bool maximize, bool fullscreen)
    : resizable_{resizable} {
    if (fullscreen && (maximize || resizable)) {
        throw std::logic_error(
            std::string{"Window: other flags not allowed while fullscreen"});
    }

    if (maximize && !resizable) {
        throw std::logic_error(
            std::string{"Window: to maximize, enable resizable also"});
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        throw std::runtime_error(std::string{SDL_GetError()});
    }

    Uint32 flags = SDL_WINDOW_OPENGL;

    if (resizable) {
        flags |= SDL_WINDOW_RESIZABLE;
    }

    if (maximize) {
        flags |= SDL_WINDOW_MAXIMIZED;
    }

    if (fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    sdl_window_.init(title.c_str(), SDL_WINDOWPOS_UNDEFINED,
                     SDL_WINDOWPOS_UNDEFINED, width, height, flags);
    sdl_gl_context_.init(sdl_window_.m_sdl_window);

    SDL_GetWindowSize(sdl_window_.m_sdl_window, &m_width, &m_height);

    if (!gladLoadGLLoader(
            reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
        throw std::runtime_error(std::string{"glad failed to load"});
    }

    fprintf(stderr, "VENDOR: %s\nRENDERER: %s\nVERSION: %s\n",
            glGetString(GL_VENDOR), glGetString(GL_RENDERER),
            glGetString(GL_VERSION));
}

void Window::tick() noexcept {
    if (resizable_) {
        int tmp_width, tmp_height;
        SDL_GetWindowSize(sdl_window_.m_sdl_window, &tmp_width, &tmp_height);
        m_window_resized = (m_width != tmp_width || m_height != tmp_height);
        m_width = tmp_width;
        m_height = tmp_height;
    }
    m_last_frame_time = m_this_frame_time;
    m_this_frame_time = SDL_GetTicks();
}

void Window::swap_buffers() noexcept {
    SDL_GL_SwapWindow(sdl_window_.m_sdl_window);
}

void Window::set_title(char const* buffer) {
    SDL_SetWindowTitle(sdl_window_.m_sdl_window, buffer);
}