#ifndef ADRENALINE_CAMERAMANAGER_H
#define ADRENALINE_CAMERAMANAGER_H

#include <functional>
#include <glm/glm.hpp>
#include <vector>

#include "camera.h"
#include "context.h"

struct CameraConfig {
    using mat4 = glm::mat4;
    using vec3 = glm::vec3;

    /* calling this function, when camera rotates. This function updates, the
     * rotation matrix, and updates the rotation values.
     *
     * r  = rotation matrix, till previous frame
     * rx = total rotation across x axis, till previous frame
     * ry = total rotation across y axis, till previous frame
     * rz = total rotation across z axis, till previous frame
     * x  = amount to rotate around x axis, in this frame
     * y  = amount to rotate around y axis, in this frame
     * z  = amount to rotate around z axis, in this frame
     * */

    using T1 =
        std::function<void(Context const& c, mat4& r, float& rx, float& ry,
                           float& rz, float x, float y, float z)>;

    /*calling this function, to calculate translation vectors (WD in WASD)
     *
     * rx  = total rotation across x axis, till this frame
     * ry  = total rotation across y axis, till this frame
     * rz  = total rotation across z axis, till this frame
     *
     * d_f = initial forward vector
     * d_r = initial right vector
     *
     * f   = should hold current frame, forward vector
     * r   = should hold current frame, right vector
     * */
    using T2 =
        std::function<void(Context const&, float rx, float ry, float rz,
                           vec3 const& d_f, vec3 const& d_r, vec3& f, vec3& r)>;

    float m_fov{};
    float m_aspect{};
    float m_z_near{};
    float m_z_far{};
    vec3 m_pos{};
    vec3 m_tar{};
    vec3 m_up{};
    bool m_updt_frstm{};
    T1 m_crp{};
    T2 m_ctv{};

    CameraConfig(float fov, float aspect, float z_near, float z_far,
                 vec3 const& pos, vec3 const& tar, vec3 const& up,
                 bool updt_frstm, T1 crp, T2 ctv) noexcept;
};

struct CameraInstance {
    using mat4 = glm::mat4;
    using vec3 = glm::vec3;

    using T1 =
        std::function<void(Context const& c, mat4& r, float& rx, float& ry,
                           float& rz, float x, float y, float z)>;

    using T2 =
        std::function<void(Context const&, float rx, float ry, float rz,
                           vec3 const& d_f, vec3 const& d_r, vec3& f, vec3& r)>;

    Camera m_camera;
    T1 m_crp;
    T2 m_ctv;

    explicit CameraInstance(CameraConfig const& config) noexcept;
};

class CameraManager {
    using vec3 = glm::vec3;
    using mat4 = glm::mat4;

   public:
    CameraManager() = default;
    size_t add(Context const& context, CameraConfig const& config);
    void set(size_t index) noexcept;  // set current camera, denoted by index
    void tick(Context const& context) noexcept;
    mat4 view() const noexcept;
    mat4 p_proj() const noexcept;
    size_t size() const noexcept { return instances_.size(); }
    void rotate(Context const& context, float x, float y, float z) noexcept;
    void translate(vec3 const& vec) noexcept;
    void translate_forward(float dt) noexcept;
    void translate_backward(float dt) noexcept;
    void translate_right(float dt) noexcept;
    void translate_left(float dt) noexcept;

    vec3 position() const noexcept {
        return instances_[index_].m_camera.position();
    }

    vec3 forward() const noexcept {
        return instances_[index_].m_camera.forward();
    }

    vec3 backward() const noexcept {
        return instances_[index_].m_camera.backward();
    }

    vec3 left() const noexcept { return instances_[index_].m_camera.left(); }

    vec3 right() const noexcept { return instances_[index_].m_camera.right(); }

    vec3 forward_axis() const noexcept {
        return instances_[index_].m_camera.forward_axis();
    }

    vec3 right_axis() const noexcept {
        return instances_[index_].m_camera.right_axis();
    }

    size_t current_camera_id() const noexcept { return index_; }

    /* return frustum vertex specified by fv_index, of curret bounded camera */
    vec3& operator[](size_t fv_index) noexcept {
        return instances_[index_].m_camera[fv_index];
    }

   private:
    size_t index_{};
    std::vector<CameraInstance> instances_;
};

#endif  // ADRENALINE_CAMERAMANAGER_H
