#include "input.h"

#include "window.h"

Input::Input() noexcept {
    m_keyboard_state.m_key_pressed_state = SDL_GetKeyboardState(nullptr);
}

int Input::set_relative_mouse_mode(bool state) noexcept {
    int ret = SDL_SetRelativeMouseMode(static_cast<SDL_bool>(state));
    if (ret == 0) m_mouse_state.m_relative_mode = state;
    return ret;
}

void Input::mouse_tick() noexcept {
    auto button_state =
        SDL_GetMouseState(&m_mouse_state.m_x, &m_mouse_state.m_y);
    SDL_GetRelativeMouseState(&m_mouse_state.m_x_dt, &m_mouse_state.m_y_dt);

    m_mouse_state.m_left = button_state & SDL_BUTTON(SDL_BUTTON_LEFT);
    m_mouse_state.m_middle = button_state & SDL_BUTTON(SDL_BUTTON_MIDDLE);
    m_mouse_state.m_right = button_state & SDL_BUTTON(SDL_BUTTON_RIGHT);
}

void Input::keyboard_clean() noexcept {
    memset(static_cast<void *>(&m_keyboard_state.m_key_up_state), 0,
           sizeof(m_keyboard_state.m_key_up_state));

    memset(static_cast<void *>(&m_keyboard_state.m_key_down_state), 0,
           sizeof(m_keyboard_state.m_key_down_state));
}

void Input::mouse_clean() noexcept {
    m_mouse_state.m_wheel_up = false;
    m_mouse_state.m_wheel_down = false;
    m_mouse_state.m_wheel_left = false;
    m_mouse_state.m_wheel_right = false;
}

void Input::tick() noexcept {
    this->keyboard_clean();
    this->mouse_clean();
    while (SDL_PollEvent(&sdl_event_)) {
        switch (sdl_event_.type) {
            case SDL_QUIT:
                m_quit_event = true;
                break;
            case SDL_KEYUP:
                m_keyboard_state
                    .m_key_up_state[sdl_event_.key.keysym.scancode] = 1;
                break;
            case SDL_KEYDOWN:
                m_keyboard_state
                    .m_key_down_state[sdl_event_.key.keysym.scancode] = 1;
                break;
            case SDL_MOUSEWHEEL:
                if (sdl_event_.wheel.y > 0) {
                    m_mouse_state.m_wheel_up = true;
                } else if (sdl_event_.wheel.y < 0) {
                    m_mouse_state.m_wheel_down = true;
                }
                if (sdl_event_.wheel.x < 0) {
                    m_mouse_state.m_wheel_left = true;
                } else if (sdl_event_.wheel.x > 0) {
                    m_mouse_state.m_wheel_right = true;
                }
                break;
        }
    }
    this->mouse_tick();
}

bool Input::is_event_occured(const Event &event) const noexcept {
    if (event.m_type == EventType::KEYPRESS) {
        return m_keyboard_state
            .m_key_pressed_state[static_cast<uint32_t>(event.m_key)];
    }
    if (event.m_type == EventType::KEYUP) {
        return m_keyboard_state
            .m_key_up_state[static_cast<uint32_t>(event.m_key)];
    }
    if (event.m_type == EventType::KEYDOWN) {
        return m_keyboard_state
            .m_key_down_state[static_cast<uint32_t>(event.m_key)];
    }
    if (event.m_type == EventType::MOUSEWHEEL) {
        if (event.m_mouse == Mouse::WHEELUP) return m_mouse_state.m_wheel_up;
        if (event.m_mouse == Mouse::WHEELDOWN)
            return m_mouse_state.m_wheel_down;
        if (event.m_mouse == Mouse::WHEELLEFT)
            return m_mouse_state.m_wheel_left;
        if (event.m_mouse == Mouse::WHEELRIGHT)
            return m_mouse_state.m_wheel_right;
    }
    return false;
}

bool Input::is_key_up_event(KEY key) const noexcept {
    return m_keyboard_state.m_key_up_state[static_cast<uint32_t>(key)];
}

bool Input::is_key_down_event(KEY key) const noexcept {
    return m_keyboard_state.m_key_down_state[static_cast<uint32_t>(key)];
}

bool Input::is_key_pressed(KEY key) const noexcept {
    return m_keyboard_state.m_key_pressed_state[static_cast<uint32_t>(key)];
}

void Input::set_mouse_position(Window &window, int x, int y) noexcept {
    SDL_WarpMouseInWindow(window.sdl_window_.m_sdl_window, x, y);
}
