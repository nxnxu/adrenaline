#include "mouse_controller.h"

void MouseController::tick(const Context &context) noexcept {
    auto x_dt = -1 * context.m_input->m_mouse_state.m_x_dt;
    auto y_dt = -1 * context.m_input->m_mouse_state.m_y_dt;

    rotate_x_dt = y_dt * (360.0f / context.m_mouse_sensitivity);
    rotate_y_dt = x_dt * (360.0f / context.m_mouse_sensitivity);
}
