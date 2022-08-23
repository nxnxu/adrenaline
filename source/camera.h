#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "context.h"

class CameraManager;

class Camera {
    using vec3 = glm::vec3;
    using vec4 = glm::vec4;
    using mat4 = glm::mat4;

    friend CameraManager;

   public:
    Camera(float fov, float aspect, float near, float far, vec3 const& p,
           vec3 const& t, vec3 const& u, bool updt_frstm) noexcept;

    void tick(Context const& context) noexcept;
    void translate(vec3 const& vec) noexcept;
    void translate_forward(float dt) noexcept;
    void translate_backward(float dt) noexcept;
    void translate_right(float dt) noexcept;
    void translate_left(float dt) noexcept;

    vec3 position() const noexcept {
        return vec3{position_} + total_translated_;
    }

    vec3 forward() const noexcept { return forward_; }

    vec3 backward() const noexcept { return forward_ * -1.0f; }

    vec3 left() const noexcept { return right_ * -1.0f; }

    vec3 right() const noexcept { return right_; }

    vec3 forward_axis() const noexcept { return forward_axis_; }

    vec3 right_axis() const noexcept { return right_axis_; }

    mat4 view() const noexcept;
    mat4 p_proj() const noexcept;

    vec3& operator[](size_t index) noexcept {
        return frustum_vertices_updt_[index];
    };

   private:
    void calculate_frustum_vertices() noexcept;
    void update_frustum_vertices() noexcept;
    void calculate_view() noexcept;
    void calculate_perspective_proj() noexcept;

    float fov_;
    float aspect_;
    float z_near_;
    float z_far_;
    vec3 position_;
    vec4 target_;
    vec4 up_;
    bool updt_frstm_;
    vec3 frustum_vertices_base_[8];
    vec3 frustum_vertices_updt_[8];

    float rotate_x{0.0f};
    float rotate_y{0.0f};
    float rotate_z{0.0f};

    mat4 view_{1.0f};
    mat4 p_proj_{1.0f};
    mat4 rot_mat_{1.0f};
    vec3 total_translated_{0.0f};

    /*
     * These represents current frame, forward and right vector, used for
     * translation
     * */
    vec3 forward_{};
    vec3 right_{};

    vec3 forward_axis_{};
    vec3 right_axis_{};
};

#endif  // CAMERA_H
