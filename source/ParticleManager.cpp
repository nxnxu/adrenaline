#include "ParticleManager.h"

using glm::mat4;
using glm::vec3;
using glm::vec4;

ParticleManager::ParticleManager(size_t max_particles)
    : points_{max_particles} {}

void ParticleManager::add(Context const& context, PMConfig const& config) {
    for (size_t g = 0; g < config.m_instance_count; ++g) {
        uint32_t instance_birth_time =
            context.m_this_frame_time + g * config.m_instance_delay;
        instances_.emplace_back(instance_birth_time, config.m_tick);
        auto& instance = instances_.back();
        for (size_t i = 0; i < config.m_num_particles; ++i) {
            auto x = config.m_allowed_axis.x;
            auto y = config.m_allowed_axis.y;
            auto z = config.m_allowed_axis.z;
            x = x * dir_dist_(random_generator.m_gen);
            y = y * dir_dist_(random_generator.m_gen);
            z = z * dir_dist_(random_generator.m_gen);
            float magnitude = glm::mix(config.m_min_vel, config.m_max_vel,
                                       mag_dist_(random_generator.m_gen));

            vec3 velocity = normalize(vec3{x, y, z}) * magnitude;

            instance.particles_.emplace_back(
                config.m_ndc, config.m_point_size, context.m_this_frame_time, 0,
                config.m_max_age, config.m_birth_pos, velocity,
                config.m_birth_pos, vec4{1.0f});
        }
    }
}

void ParticleManager::tick(Context const& context, PrimitiveManager& pm) {
    point_index_ = 0;

    size_t i = 0;
    size_t j = instances_.size();

    while (i < j) {
        if (context.m_this_frame_time < instances_[i].birth_time_) {
            ++i;
            continue;
        }

        auto& particles = instances_[i].particles_;

        size_t pi = 0;
        size_t pj = particles.size();

        while (pi < pj) {
            instances_[i].m_particle_tick_(context, particles[pi]);
            if (particles[pi].m_age >= particles[pi].m_max_age) {
                std::swap(particles[pi], particles[pj - 1]);
                --pj;
            } else {
                ++pi;
            }
        }
        particles.erase(particles.begin() + pj, particles.end());

        if (particles.empty()) {
            std::swap(instances_[i], instances_[j - 1]);
            --j;
        } else {
            for (size_t k = 0;
                 k < particles.size() && point_index_ < points_.size();
                 ++k, ++point_index_) {
                points_[point_index_].m_ndc = particles[k].m_ndc;
                points_[point_index_].m_point_size = particles[k].m_point_size;
                points_[point_index_].m_position = particles[k].m_pos;
                points_[point_index_].m_color = particles[k].m_color;
            }
            ++i;
        }
    }

    instances_.erase(instances_.begin() + j, instances_.end());
    if (point_index_) {
        pm.register_copy_to_temporary_buffer(&points_[0], point_index_);
    }
}

PMConfig::PMConfig(size_t count, uint32_t delay, bool ndc,
                   vec3 const& birth_pos, float min_vel, float max_vel,
                   size_t num_particles, vec3 allowed_axis, uint32_t point_size,
                   uint32_t max_age, tick_type tick)
    : m_instance_count{count},
      m_instance_delay{delay},
      m_ndc{ndc},
      m_birth_pos{birth_pos},
      m_min_vel{min_vel},
      m_max_vel{max_vel},
      m_num_particles{num_particles},
      m_allowed_axis{allowed_axis},
      m_point_size{point_size},
      m_max_age{max_age},
      m_tick{std::move(tick)} {}

Particle::Particle(bool ndc, uint32_t point_size, uint32_t birth_time,
                   uint32_t age, uint32_t max_age, vec3 const& pos,
                   vec3 const& velocity, vec3 const& birth_pos,
                   vec4 const& color) noexcept

    : m_ndc{ndc},
      m_point_size{point_size},
      m_birth_time{birth_time},
      m_age{age},
      m_max_age{max_age},
      m_pos{pos},
      m_velocity{velocity},
      m_birth_pos{birth_pos},
      m_color{color} {}
