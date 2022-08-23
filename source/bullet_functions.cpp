#include "bullet_functions.h"

using glm::vec3;
using glm::vec4;

inline void common_bullet_movement(Context const &context,
                                   Bullet &bullet) noexcept {
    bullet.m_old_pos = bullet.m_current_pos;
    float dt = (1.0f / 1000.0) * context.m_dt;
    bullet.m_current_pos += bullet.m_velocity * dt;
}

void BULLET_WL45_TICK(Context const &context, Bullet &bullet,
                      ParticleManager &pm) {
    common_bullet_movement(context, bullet);

    // TODO: Negative direction ?
    // Add direct direction vector alternative to PMConfig::VEL_XYZ*
    static PMConfig config{1,
                           0,
                           false,
                           vec3{0.0f},
                           0.1f,
                           0.5f,
                           100,
                           vec3{1.0},
                           2,
                           100,
                           PARTICLE_TICK_NORMAL_FIRE};

    config.m_birth_pos = bullet.m_current_pos;
    pm.add(context, config);
}

void BULLET_EB95_TICK(Context const &context, Bullet &bullet,
                      ParticleManager &pm) {
    common_bullet_movement(context, bullet);

    static PMConfig config{1,
                           0,
                           false,
                           vec3{0.0f},
                           0.1f,
                           0.1f,
                           1000,
                           vec3{1.0},
                           1,
                           500,
                           PARTICLE_TICK_NORMAL_WHITE};

    config.m_birth_pos = bullet.m_current_pos;
    pm.add(context, config);
}

void BULLET_RL96_TICK(Context const &context, Bullet &bullet,
                      ParticleManager &pm) {
    common_bullet_movement(context, bullet);

    static PMConfig config{1,
                           0,
                           false,
                           vec3{0.0f},
                           0.1f,
                           0.5f,
                           100,
                           vec3{1.0},
                           2,
                           100,
                           PARTICLE_TICK_NORMAL_RED};

    config.m_birth_pos = bullet.m_current_pos;
    pm.add(context, config);
}

void BULLET_WL45_HIT(Context const &context, Bullet &bullet, vec3 const &hit,
                     ParticleManager &pm) {
    (void)bullet;
    static PMConfig config{1,
                           0,
                           false,
                           vec3{0.0f},
                           0.1f,
                           1.0f,
                           100,
                           vec3{1.0},
                           10,
                           100,
                           PARTICLE_TICK_NORMAL_RED};

    config.m_birth_pos = hit;
    pm.add(context, config);
}

void BULLET_EB95_HIT(Context const &context, Bullet &bullet, vec3 const &hit,
                     ParticleManager &pm) {
    (void)bullet;
    static PMConfig config{10,
                           10,
                           false,
                           vec3{0.0f},
                           0.1f,
                           1.0f,
                           1000,
                           vec3{1.0},
                           5,
                           1000,
                           PARTICLE_TICK_NORMAL_FIRE};

    config.m_birth_pos = hit;
    pm.add(context, config);
}

void BULLET_RL96_HIT(Context const &context, Bullet &bullet, vec3 const &hit,
                     ParticleManager &pm) {
    (void)bullet;
    static PMConfig config{1,
                           0,
                           false,
                           vec3{0.0f},
                           0.1f,
                           1.0f,
                           100,
                           vec3{1.0},
                           10,
                           100,
                           PARTICLE_TICK_NORMAL_RED};

    config.m_birth_pos = hit;
    pm.add(context, config);
}
