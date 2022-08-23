#ifndef ADRENALINE_PARTICLEMANAGER_H
#define ADRENALINE_PARTICLEMANAGER_H

#include <functional>
#include <glm/glm.hpp>
#include <vector>

#include "PrimitiveManager.h"
#include "context.h"
#include "random_generator.h"

struct Particle {
    using vec3 = glm::vec3;
    using vec4 = glm::vec4;

    bool m_ndc;
    uint32_t m_point_size;
    uint32_t m_birth_time;
    uint32_t m_age;
    uint32_t m_max_age;

    glm::vec3 m_pos;
    glm::vec3 m_velocity;
    glm::vec3 m_birth_pos;
    glm::vec4 m_color;

    Particle(bool ndc, uint32_t point_size, uint32_t birth_time, uint32_t age,
             uint32_t max_age, vec3 const& pos, vec3 const& velocity,
             vec3 const& birth_pos, vec4 const& color) noexcept;
};

struct PMConfig {
    using vec3 = glm::vec3;
    using vec4 = glm::vec4;
    using tick_type = std::function<void(Context const&, Particle&)>;

    size_t m_instance_count;
    uint32_t m_instance_delay;
    bool m_ndc;
    vec3 m_birth_pos;
    float m_min_vel;
    float m_max_vel;
    size_t m_num_particles;
    vec3 m_allowed_axis;
    uint32_t m_point_size;
    uint32_t m_max_age;
    tick_type m_tick;

    PMConfig(size_t count, uint32_t delay, bool ndc, vec3 const& birth_pos,
             float min_vel, float max_vel, size_t num_particles,
             vec3 allowed_axis, uint32_t point_size, uint32_t max_age,
             tick_type tick);
};

class ParticleManager;

class ParticleInstance {
    friend ParticleManager;

   public:
    explicit ParticleInstance(uint32_t birth_time,
                              PMConfig::tick_type particle_tick)
        : birth_time_{birth_time}, m_particle_tick_{std::move(particle_tick)} {}

   private:
    uint32_t birth_time_{};
    PMConfig::tick_type m_particle_tick_;
    std::vector<Particle> particles_;
};

class ParticleManager {
    using vec3 = glm::vec3;
    using vec4 = glm::vec4;

   public:
    explicit ParticleManager(size_t max_particles);
    void add(Context const& context, PMConfig const& config);
    void tick(Context const& context, PrimitiveManager& pm);
    ParticleManager(ParticleManager const&) = delete;
    ParticleManager(ParticleManager&&) = delete;
    size_t size() const noexcept { return instances_.size(); }

    void reset() noexcept { instances_.clear(); }

   private:
    size_t point_index_{};
    std::vector<ParticleInstance> instances_;
    std::vector<PrimitiveManager::Vertex> points_;

    std::uniform_real_distribution<float> dir_dist_{-1.0f, 1.0f};
    std::uniform_real_distribution<float> mag_dist_{0.0f, 1.0f};
};

#endif  // ADRENALINE_PARTICLEMANAGER_H
