#ifndef SDLGLCONTEXT_H
#define SDLGLCONTEXT_H

#include <SDL.h>

#include <exception>
#include <string>

class SDLGLContext {
   public:
    SDL_GLContext m_sdl_gl_context{};
    SDLGLContext() = default;
    SDLGLContext(SDLGLContext const &) = delete;
    SDLGLContext(SDLGLContext &&) noexcept;
    void init(SDL_Window *sdl_window);
    ~SDLGLContext();

   private:
    void initialize_gl_attributes();
};

#endif  // SDLGLCONTEXT_H
