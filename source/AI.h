#ifndef ADRENALINE_AI_H
#define ADRENALINE_AI_H

#include <glm/glm.hpp>

#include "AIModel.h"
#include "bullet.h"
#include "bullet_functions.h"
#include "collision.h"
#include "context.h"
#include "drawing_obj.h"
#include "geometrics.h"
#include "helper.h"
#include "maze.h"
#include "weapon.h"

class AIManager;

struct AIConfig {
    using vec3 = glm::vec3;

    using T1 = std::function<Bullet(Context const&, vec3 const& origin,
                                    vec3 const& forward, vec3 const& right)>;

    size_t m_initial_health;
    float m_angular_speed;
    float m_speed;
    vec3 m_nozzle_origin;
    vec3 m_forward;
    AABB m_aabb;
    AIModel* m_ai_model;
    Weapon::Weapon m_weapon;
    float m_space_radius;

    AIConfig(size_t health, float angular_speed, float speed,
             vec3 nozzle_origin, vec3 forward, AABB aabb, AIModel* ai_model,
             Weapon::Weapon weapon, float space_radius)
        : m_initial_health{health},
          m_angular_speed{angular_speed},
          m_speed{speed},
          m_nozzle_origin{nozzle_origin},
          m_forward{forward},
          m_aabb{aabb},
          m_ai_model{ai_model},
          m_weapon{std::move(weapon)},
          m_space_radius{space_radius} {}
};

class AI {
    using vec3 = glm::vec3;
    using mat4 = glm::mat4;

    friend AIManager;

   public:
    AI(AIConfig const& config) noexcept;

    void tick(Context const& context, DrawingObj& dwo, Maze& maze,
              vec3 const& player_pos, vec3 const& eye_pos,
              std::vector<Bullet>& ai_bullets);

    void translate_to_node_center(size_t node, vec3 const& vec) noexcept;
    void translate(vec3 const& vec) noexcept;
    void do_translation_job(Context const& context, Maze& maze) noexcept;
    bool can_see_player(Context const& context, Maze& maze,
                        vec3 const& player_pos) noexcept;
    void rotate_head_towards_player(Context const& context,
                                    glm::vec3 const& ai_to_player) noexcept;

    void bullet_hit(size_t bullet_damage) noexcept {
        health_ -= std::min(health_, bullet_damage);
    }

    size_t health() noexcept { return health_; }
    size_t health() const noexcept { return health_; }

    vec3 const& position() const noexcept { return pos_; }
    vec3 const& position() noexcept { return pos_; }
    AABB const& aabb() noexcept { return aabb_; }

   private:
    size_t health_;   // current health of AI
    vec3 pos_{0.0f};  // AI's position
                      // Local position of the nozzle. (No Transformation)
    vec3 base_nozzle_origin;  // this position is used as origin for firing
                              // bullet

    vec3 curr_nozzle_origin;  // current frames, nozzle position, with applied
                              // rot matrix and all bullets are fired from this
                              // position

    vec3 forward_;         // AI's heads forward vector
    vec3 right_;           // AI's heads right vector
    mat4 head_rot_{1.0f};  // hold rotation matrix for AI's head
    AABB aabb_;            // AABB for AI
    float angular_speed_;  // number of degree, rotations per second
    float speed_;          // speed of AI per second

    struct TranslationTask {
        size_t m_rst_node{
            std::numeric_limits<size_t>::max()};  // when AI reaches this node,
                                                  // it stays there and stops
                                                  // going any further
        size_t m_cur_node{};                      // last node AI visited
        size_t m_nxt_node{};                      // next node AI travels too.
        size_t m_dst_node{};                      // AI destination node

        float m_dist{};  // remaing distance to travel, such that AI reaches
                         // next node from curr node.
        vec3 m_dir{};    // direction from current node to next node

    } translation_task_;

    Weapon::Weapon weapon_;
    float space_radius_;  // personal space radius of AI, if player is within
                          // this radius,then AI should stop translating
};

#endif  // ADRENALINE_AI_H
