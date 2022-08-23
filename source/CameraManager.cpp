#include "CameraManager.h"

using glm::mat4;
using glm::vec3;
using glm::vec4;

size_t CameraManager::add(Context const &context, CameraConfig const &config) {
    (void)context;
    instances_.emplace_back(config);
    return size() - 1;
}

void CameraManager::set(size_t index) noexcept {
    if (index >= size()) {
        assert(0);
    }

    index_ = index;
}

void CameraManager::rotate(Context const &context, float x, float y,
                           float z) noexcept {
    instances_[index_].m_crp(context, instances_[index_].m_camera.rot_mat_,
                             instances_[index_].m_camera.rotate_x,
                             instances_[index_].m_camera.rotate_y,
                             instances_[index_].m_camera.rotate_z, x, y, z);

    vec3 default_forward = instances_[index_].m_camera.target_;

    vec3 default_right = glm::cross(vec3(instances_[index_].m_camera.target_),
                                    vec3(instances_[index_].m_camera.up_));

    instances_[index_].m_ctv(context, instances_[index_].m_camera.rotate_x,
                             instances_[index_].m_camera.rotate_y,
                             instances_[index_].m_camera.rotate_z,
                             default_forward, default_right,
                             instances_[index_].m_camera.forward_,
                             instances_[index_].m_camera.right_);

    instances_[index_].m_camera.forward_axis_ =
        instances_[index_].m_camera.rot_mat_ * vec4(default_forward, 0.0f);

    instances_[index_].m_camera.right_axis_ =
        instances_[index_].m_camera.rot_mat_ * vec4(default_right, 0.0f);
}

mat4 CameraManager::view() const noexcept {
    return instances_[index_].m_camera.view();
}

mat4 CameraManager::p_proj() const noexcept {
    return instances_[index_].m_camera.p_proj();
}

void CameraManager::tick(Context const &context) noexcept {
    instances_[index_].m_camera.tick(context);
}

void CameraManager::translate(const CameraManager::vec3 &vec) noexcept {
    instances_[index_].m_camera.translate(vec);
}

void CameraManager::translate_forward(float dt) noexcept {
    instances_[index_].m_camera.translate_forward(dt);
}

void CameraManager::translate_backward(float dt) noexcept {
    instances_[index_].m_camera.translate_backward(dt);
}

void CameraManager::translate_right(float dt) noexcept {
    instances_[index_].m_camera.translate_right(dt);
}

void CameraManager::translate_left(float dt) noexcept {
    instances_[index_].m_camera.translate_left(dt);
}

CameraConfig::CameraConfig(float fov, float aspect, float z_near, float z_far,
                           vec3 const &pos, vec3 const &tar, vec3 const &up,
                           bool updt_frstm, T1 crp, T2 ctv) noexcept

    : m_fov{fov},
      m_aspect{aspect},
      m_z_near{z_near},
      m_z_far{z_far},
      m_pos{pos},
      m_tar{tar},
      m_up{up},
      m_updt_frstm{updt_frstm},
      m_crp{std::move(crp)},
      m_ctv{std::move(ctv)} {}

CameraInstance::CameraInstance(CameraConfig const &config) noexcept
    : m_camera{config.m_fov,   config.m_aspect,    config.m_z_near,
               config.m_z_far, config.m_pos,       config.m_tar,
               config.m_up,    config.m_updt_frstm}

      ,
      m_crp{config.m_crp},
      m_ctv{config.m_ctv} {}
