#ifndef ADRENALINE_PLAYER_H
#define ADRENALINE_PLAYER_H

#include <fstream>
#include <glm/glm.hpp>
#include <string>

#include "CameraManager.h"
#include "collision.h"
#include "context.h"
#include "crp_functions.h"
#include "ctv_functions.h"
#include "geometrics.h"
#include "helper.h"
#include "importor.h"
#include "maze.h"
#include "mouse_controller.h"

struct PlayerConfig {
    using vec3 = glm::vec3;
    size_t m_initial_health{};
    AABB m_aabb{};
    AABBXZ m_aabbxz{};
    float m_walk_speed{};
    float m_run_speed{};
    CameraConfig camera_config;

    PlayerConfig(size_t health, AABB const& aabb, AABBXZ const& aabbxz,
                 float walk_speed, float run_speed, CameraConfig const& config)
        : m_initial_health{health},
          m_aabb{aabb},
          m_aabbxz{aabbxz},
          m_walk_speed{walk_speed},
          m_run_speed{run_speed},
          camera_config{config} {}
};

class Player {
    using vec3 = glm::vec3;

   public:
    explicit Player(Context const&, PlayerConfig const&, CameraManager&);
    size_t health() const noexcept { return health_; }

    void bullet_hit(size_t bullet_damage) noexcept {
        health_ -= std::min(health_, bullet_damage);
    }

    void tick(Context const&, Maze&, CameraManager& cm);
    void translate_no_collision_checking(vec3 const& vec,
                                         CameraManager&) noexcept;
    size_t cam_id() const noexcept { return cam_id_; }

    void add_health(size_t health) noexcept { health_ += health; }

    vec3 position() const noexcept { return position_; }

    AABB const& aabb() noexcept { return aabb_; }

   private:
    void process_player_movement(Context const&, Maze& maze, CameraManager& cm);

    size_t health_;
    AABB aabb_{};
    AABBXZ aabbxz_{};
    vec3 position_{0.0f};
    float walk_speed_{};
    float run_speed_{};
    size_t cam_id_{};
};

#endif  // ADRENALINE_PLAYER_H
