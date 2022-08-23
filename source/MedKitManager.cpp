#include "MedKitManager.h"

MedKitManager::MedKitManager(Context const& context, Maze& maze)
    : med_kits_per_maze{static_cast<size_t>(sqrt(1.0f * maze.PN * maze.PM))} {
    for (size_t i = 0; i < med_kits_per_maze; ++i) {
        med_kits_[0].emplace_back(
            spawn_med_kit(context.m_this_frame_time, maze,
                          rnd_dist_(random_generator.m_gen)));
    }
}

size_t MedKitManager::tick(Context const& context, Maze& maze,
                           ParticleManager& pm, glm::vec3 const& player_pos) {
    size_t picked_health = 0;
    for (auto& it : med_kits_) {
        for (auto& med_kit : it.second) {
            /*medkit not available yet*/
            if (med_kit.m_birth_time > context.m_this_frame_time) {
                continue;
            }

            /*player is near med_kit, pick it*/
            if (glm::length(med_kit.m_pos - player_pos) <= pick_radius_) {
                picked_health += med_kit.m_health;
                /*now respawn another medkit in some other location, which will
                 * be available after 1 second*/
                med_kit = spawn_med_kit(context.m_this_frame_time + 1000, maze,
                                        rnd_dist_(random_generator.m_gen));
            }
        }
    }

    /*add particle effect over medkits, so its visible*/

    for (auto& it : med_kits_) {
        for (auto& med_kit : it.second) {
            /*medkit not available yet*/
            if (med_kit.m_birth_time > context.m_this_frame_time) {
                continue;
            }

            static PMConfig config{
                1,                // number of instance to launch
                0,                // delay between instances
                false,            // ndc
                glm::vec3{0.0f},  // birth position
                0.1f,             // minimum velocity
                5.0f,             // max velocity
                10,               // number of particles per instance
                glm::vec3{1.0},   // allowed axis for velocity
                1,                // point size
                100,              // maximum age of particle
                PARTICLE_TICK_NORMAL_YELLOW};  // particle tick function

            config.m_num_particles = med_kit.m_health;
            config.m_birth_pos = med_kit.m_pos;
            config.m_birth_pos.y = 0.3f;

            pm.add(context, config);
        }
    }

    return picked_health;
}

void MedKitManager::add_med_kit(uint32_t birth_time, Maze& maze, size_t health,
                                glm::vec3 const& pos) noexcept {
    /*replace fisrt lowest health, with this*/

    (void)maze;

    size_t id = 0;
    size_t lowest_health = std::numeric_limits<size_t>::max();

    auto& kits = med_kits_[0];

    for (size_t i = 0; i < kits.size(); ++i) {
        if (kits[i].m_health < lowest_health) {
            id = i;
            lowest_health = kits[i].m_health;
        }
    }

    med_kits_[0][id] = MedKit{birth_time, health, pos};
}

MedKit MedKitManager::spawn_med_kit(uint32_t birth_time, Maze& maze,
                                    size_t health) noexcept {
    auto node = maze.get_random_node();
    auto node_pos = maze.node_pos_center(node);
    return MedKit{birth_time, health, node_pos};
}

void MedKitManager::mark_medkit_in_minimap(Context const& context,
                                           MiniMap& minimap) noexcept {
    for (auto& it : med_kits_) {
        for (auto& med_kit : it.second) {
            /*medkit not available yet*/
            if (med_kit.m_birth_time > context.m_this_frame_time) {
                continue;
            }

            minimap.add_temporary(5, med_kit.m_pos,
                                  glm::vec4{0.3f, 0.2f, 0.0f, 1.0f});
        }
    }
}

void MedKitManager::reset(Context const& context, Maze& maze) noexcept {
    for (auto& it : med_kits_) {
        for (auto& med_kit : it.second) {
            med_kit = spawn_med_kit(context.m_this_frame_time, maze,
                                    rnd_dist_(random_generator.m_gen));
        }
    }
}