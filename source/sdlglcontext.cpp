#include "sdlglcontext.h"

#include <stdexcept>

void SDLGLContext::init(SDL_Window* sdl_window) {
    initialize_gl_attributes();
    m_sdl_gl_context = SDL_GL_CreateContext(sdl_window);
    if (m_sdl_gl_context == nullptr) {
        throw std::runtime_error(std::string{SDL_GetError()});
    }
}

void SDLGLContext::initialize_gl_attributes() {
    bool ret = (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) == 0 &&
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) == 0 &&
                SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24) == 0 &&
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                                    SDL_GL_CONTEXT_PROFILE_CORE) == 0 &&
                SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8) == 0);
    if (ret == false) {
        throw std::runtime_error(std::string{SDL_GetError()});
    }
}

SDLGLContext::SDLGLContext(SDLGLContext&& context) noexcept
    : m_sdl_gl_context{context.m_sdl_gl_context} {
    context.m_sdl_gl_context = nullptr;
}

SDLGLContext::~SDLGLContext() { SDL_GL_DeleteContext(m_sdl_gl_context); }
