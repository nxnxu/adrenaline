#ifndef GAME_CONTEXT_H
#define GAME_CONTEXT_H

#include <glm/glm.hpp>
#include <random>

#include "actions.h"

class MouseController;
class Input;

struct Context {
    using mat4 = glm::mat4;

    mat4 m_vp;
    mat4 m_view;
    mat4 m_proj;
    Input *m_input;
    MouseController *m_mouse_controller;
    Actions m_actions;
    uint32_t m_this_frame_time{};
    uint32_t m_dt{};
    uint32_t m_weapon_switch_dt{};
    uint32_t
        m_mouse_sensitivity{};  // number of mouse units per full 360 rotation
    bool m_window_resized{};
    int m_window_width{};
    int m_window_height{};
    uint32_t m_fps{};
    Window *m_window{};

    Context();
    void tick() noexcept;

   private:
    constexpr static size_t size_ = 10;

    /*holds frame dt of last size_ frames*/
    uint32_t frames_dt_[size_]{};

    /*sum of frames_dt_ array*/
    uint32_t frame_dt_sum_{};

    /*stores oldest frames_dt index, intialize value is 0*/
    size_t index_{};
};

#endif  // GAME_CONTEXT_H
