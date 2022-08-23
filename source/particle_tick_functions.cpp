#include "particle_tick_functions.h"

using glm::vec3;
using glm::vec4;

void common_particle_movement(Context const &context,
                              Particle &particle) noexcept {
    uint32_t advance_life =
        std::min(context.m_dt, particle.m_max_age - particle.m_age);
    particle.m_age += advance_life;

    /*convert ms to floating point, 1000ms = 1.0f*/
    float dt = (1.0f / 1000.0f) * advance_life;

    particle.m_pos.x = particle.m_pos.x + particle.m_velocity.x * dt;
    particle.m_pos.y = particle.m_pos.y + particle.m_velocity.y * dt;
    particle.m_pos.z = particle.m_pos.z + particle.m_velocity.z * dt;
}

void PARTICLE_TICK_NORMAL_WHITE(Context const &context,
                                Particle &particle) noexcept {
    common_particle_movement(context, particle);
    particle.m_color = vec4{1.0f, 1.0f, 1.0f, 1.0f};
}

void PARTICLE_TICK_NORMAL_RED(Context const &context,
                              Particle &particle) noexcept {
    common_particle_movement(context, particle);
    particle.m_color = vec4{0.5f, 0.0f, 0.0f, 1.0f};
}

void PARTICLE_TICK_NORMAL_YELLOW(Context const &context,
                                 Particle &particle) noexcept {
    common_particle_movement(context, particle);
    particle.m_color = vec4{0.6f, 0.5f, 0.0f, 1.0f};
}

void PARTICLE_TICK_NORMAL_FIRE(Context const &context,
                               Particle &particle) noexcept {
    common_particle_movement(context, particle);

    // life percentage, (0.0) = 0 percent life used, (1.0) = 100 percentage life
    // used
    float f_life_percent =
        (1.0f * particle.m_age) / (1.0f * particle.m_max_age);

    // reverse life percentage (0.0) = 100 percent life used, (1.0) = 0 percent
    // life used
    float r_life_percent = (1.0f - f_life_percent);

    particle.m_color.x = f_life_percent > 0.5f ? 0.5f : r_life_percent;
    particle.m_color.y = r_life_percent;
    particle.m_color.z = 0.0f;
    particle.m_color.w = 1.0f;
}