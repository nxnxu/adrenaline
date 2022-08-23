#include "camera.h"

using glm::mat4;
using glm::vec3;

Camera::Camera(float fov, float aspect, float near, float far, vec3 const& p,
               vec3 const& t, vec3 const& u, bool updt_frstm) noexcept
    : fov_{fov},
      aspect_{aspect},
      z_near_{near},
      z_far_{far},
      position_{p},
      target_{t, 0.0f},
      up_{u, 0.0f},
      updt_frstm_{updt_frstm},
      forward_{target_},
      right_{glm::cross(vec3(target_), vec3(up_))},
      forward_axis_{forward_},
      right_axis_{right_} {
    calculate_view();
    calculate_perspective_proj();

    if (updt_frstm_) {
        calculate_frustum_vertices();
        update_frustum_vertices();
    }
}

void Camera::calculate_view() noexcept {
    vec3 position = position_ + total_translated_;
    vec3 target = rot_mat_ * target_;
    vec3 upaxis = rot_mat_ * up_;
    view_ = glm::lookAt(position, position + target, upaxis);
}

void Camera::calculate_perspective_proj() noexcept {
    p_proj_ = glm::perspective(fov_, aspect_, z_near_, z_far_);
}

void Camera::tick(Context const& context) noexcept {
    calculate_view();

    if (context.m_window_resized) {
        aspect_ =
            (1.0f * context.m_window_width) / (1.0f * context.m_window_height);
        calculate_perspective_proj();

        if (updt_frstm_) {
            calculate_frustum_vertices();
        }
    }

    if (updt_frstm_) {
        update_frustum_vertices();
    }
}

void Camera::calculate_frustum_vertices() noexcept {
    float fovT = glm::tan(glm::radians(fov_) * 0.5f);

    float n_plne_hf_hgt = fovT * z_near_;
    float n_plne_hf_wdt = n_plne_hf_hgt * aspect_;

    float f_plne_hf_hgt = fovT * z_far_;
    float f_plne_hf_wdt = f_plne_hf_hgt * aspect_;

    vec3 p = position_;
    vec3 t = target_;
    vec3 u = up_;

    vec3 z_axis = glm::normalize(p - t);
    vec3 x_axis = glm::normalize(glm::cross(u, z_axis));
    vec3 y_axis = glm::normalize(glm::cross(z_axis, x_axis));

    vec3 n_plne_center = p - z_axis * z_near_;
    vec3 f_plne_center = p - z_axis * z_far_;

    /*
     *   Near Plane vertices
     *     3   2
     *     0   1
     *   Far Plane vertices
     *     7   6
     *     4   5
     * */

    frustum_vertices_base_[0] =
        n_plne_center - y_axis * n_plne_hf_hgt - x_axis * n_plne_hf_wdt;
    frustum_vertices_base_[1] =
        n_plne_center - y_axis * n_plne_hf_hgt + x_axis * n_plne_hf_wdt;
    frustum_vertices_base_[2] =
        n_plne_center + y_axis * n_plne_hf_hgt + x_axis * n_plne_hf_wdt;
    frustum_vertices_base_[3] =
        n_plne_center + y_axis * n_plne_hf_hgt - x_axis * n_plne_hf_wdt;

    frustum_vertices_base_[4] =
        f_plne_center - y_axis * f_plne_hf_hgt - x_axis * f_plne_hf_wdt;
    frustum_vertices_base_[5] =
        f_plne_center - y_axis * f_plne_hf_hgt + x_axis * f_plne_hf_wdt,
    frustum_vertices_base_[6] =
        f_plne_center + y_axis * f_plne_hf_hgt + x_axis * f_plne_hf_wdt;
    frustum_vertices_base_[7] =
        f_plne_center + y_axis * f_plne_hf_hgt - x_axis * f_plne_hf_wdt;
}

void Camera::update_frustum_vertices() noexcept {
    for (size_t i = 0; i < 8; ++i) {
        frustum_vertices_updt_[i] =
            rot_mat_ * vec4(frustum_vertices_base_[i], 0.0f);
        frustum_vertices_updt_[i] += total_translated_;
    }
}

void Camera::translate(vec3 const& vec) noexcept { total_translated_ += vec; }

mat4 Camera::view() const noexcept { return view_; }

mat4 Camera::p_proj() const noexcept { return p_proj_; }

void Camera::translate_forward(float dt) noexcept { translate(forward_ * dt); }

void Camera::translate_backward(float dt) noexcept {
    translate(forward_ * dt * -1.0f);
}

void Camera::translate_right(float dt) noexcept { translate(right_ * dt); }

void Camera::translate_left(float dt) noexcept {
    translate(right_ * dt * -1.0f);
}
