#ifndef ADRENALINE_BULLET_H
#define ADRENALINE_BULLET_H

#include <glm/glm.hpp>

#include "ParticleManager.h"
#include "context.h"

struct Bullet {
    using vec3 = glm::vec3;
    using tick_t =
        std::function<void(Context const&, Bullet&, ParticleManager&)>;
    using hit_t = std::function<void(Context const&, Bullet&, vec3 const& hit,
                                     ParticleManager&)>;

    vec3 m_birth_pos;
    vec3 m_old_pos;
    vec3 m_current_pos;
    vec3 m_velocity;
    size_t m_damage;
    float m_range;
    tick_t m_tick;
    hit_t m_hit;

    Bullet(vec3 const& pos, size_t damage, float range, vec3 const& velocity,
           tick_t tick, hit_t hit) noexcept
        : m_birth_pos{pos},
          m_old_pos{pos},
          m_current_pos{pos},
          m_velocity{velocity},
          m_damage{damage},
          m_range{range},
          m_tick{std::move(tick)},
          m_hit{std::move(hit)} {}
};

#endif  // ADRENALINE_BULLET_H
