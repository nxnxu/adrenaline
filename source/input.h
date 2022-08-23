#ifndef INPUT_H
#define INPUT_H

#include <SDL.h>

#include <exception>
#include <string>

enum class KEY : uint32_t {
    NUM_0 = SDL_SCANCODE_0,
    NUM_1 = SDL_SCANCODE_1,
    NUM_2 = SDL_SCANCODE_2,
    NUM_3 = SDL_SCANCODE_3,
    NUM_4 = SDL_SCANCODE_4,
    NUM_5 = SDL_SCANCODE_5,
    NUM_6 = SDL_SCANCODE_6,
    NUM_7 = SDL_SCANCODE_7,
    NUM_8 = SDL_SCANCODE_8,
    NUM_9 = SDL_SCANCODE_9,
    A = SDL_SCANCODE_A,
    B = SDL_SCANCODE_B,
    C = SDL_SCANCODE_C,
    D = SDL_SCANCODE_D,
    E = SDL_SCANCODE_E,
    F = SDL_SCANCODE_F,
    G = SDL_SCANCODE_G,
    H = SDL_SCANCODE_H,
    I = SDL_SCANCODE_I,
    J = SDL_SCANCODE_J,
    K = SDL_SCANCODE_K,
    L = SDL_SCANCODE_L,
    M = SDL_SCANCODE_M,
    N = SDL_SCANCODE_N,
    O = SDL_SCANCODE_O,
    P = SDL_SCANCODE_P,
    Q = SDL_SCANCODE_Q,
    R = SDL_SCANCODE_R,
    S = SDL_SCANCODE_S,
    T = SDL_SCANCODE_T,
    U = SDL_SCANCODE_U,
    V = SDL_SCANCODE_V,
    W = SDL_SCANCODE_W,
    X = SDL_SCANCODE_X,
    Y = SDL_SCANCODE_Y,
    Z = SDL_SCANCODE_Z,
    TAB = SDL_SCANCODE_TAB,
    SPACE = SDL_SCANCODE_SPACE,
    LSHIFT = SDL_SCANCODE_LSHIFT,
    RSHIFT = SDL_SCANCODE_RSHIFT,
    RETURN = SDL_SCANCODE_RETURN,
    ESCAPE = SDL_SCANCODE_ESCAPE,
    BACKSPACE = SDL_SCANCODE_BACKSPACE,
};

enum class Mouse : uint8_t {
    LEFT,
    MIDDLE,
    RIGHT,
    WHEELUP,
    WHEELDOWN,
    WHEELLEFT,
    WHEELRIGHT
};

enum class EventType : uint8_t {
    KEYUP,
    KEYDOWN,
    KEYPRESS,
    MOUSEBUTTON,
    MOUSEWHEEL
};

struct Event {
    EventType m_type;
    union {
        KEY m_key;
        Mouse m_mouse;
    };
};

class Window;

struct MouseState {
    int m_x{};
    int m_y{};
    int m_x_dt{};
    int m_y_dt{};
    bool m_left{};
    bool m_middle{};
    bool m_right{};
    bool m_wheel_up{};
    bool m_wheel_down{};
    bool m_wheel_left{};
    bool m_wheel_right{};
    bool m_relative_mode{};
};

struct KeyboardState {
    Uint8 m_key_up_state[SDL_NUM_SCANCODES]{};
    Uint8 m_key_down_state[SDL_NUM_SCANCODES]{};
    const Uint8* m_key_pressed_state{};
};

class Input {
   public:
    bool m_quit_event{};
    MouseState m_mouse_state{};
    KeyboardState m_keyboard_state{};
    Input() noexcept;
    void tick() noexcept;
    bool is_event_occured(Event const& event) const noexcept;
    void set_mouse_position(Window& window, int x, int y) noexcept;
    int set_relative_mouse_mode(bool state) noexcept;
    bool is_key_up_event(KEY key) const noexcept;
    bool is_key_down_event(KEY key) const noexcept;
    bool is_key_pressed(KEY key) const noexcept;

   private:
    void mouse_tick() noexcept;
    void keyboard_clean() noexcept;
    void mouse_clean() noexcept;
    SDL_Event sdl_event_{};
};

#endif  // INPUT_H
