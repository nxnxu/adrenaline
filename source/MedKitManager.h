#ifndef ADRENALINE_MEDKITMANAGER_H
#define ADRENALINE_MEDKITMANAGER_H

#include <glm/glm.hpp>
#include <map>
#include <random>
#include <vector>

#include "MiniMap.h"
#include "ParticleManager.h"
#include "context.h"
#include "maze.h"
#include "particle_tick_functions.h"

struct MedKit {
    uint32_t m_birth_time;
    size_t m_health;
    glm::vec3 m_pos;

    MedKit(uint32_t birth_time, size_t health, glm::vec3 pos)
        : m_birth_time{birth_time}, m_health{health}, m_pos{pos} {}
};

class MedKitManager {
   public:
    MedKitManager(Context const& context, Maze& maze);
    /*return amount of health, player collected*/
    size_t tick(Context const& context, Maze& maze, ParticleManager& pm,
                glm::vec3 const& player_pos);
    void add_med_kit(uint32_t birth_time, Maze& maze, size_t health,
                     glm::vec3 const& pos) noexcept;
    size_t med_kits_per_maze;

    void reset(Context const& context, Maze& maze) noexcept;

    void mark_medkit_in_minimap(Context const& context,
                                MiniMap& minimap) noexcept;

   private:
    MedKit spawn_med_kit(uint32_t birth_time, Maze& maze,
                         size_t health) noexcept;
    std::map<size_t, std::vector<MedKit>> med_kits_;
    float pick_radius_ = 1.0f;
    std::uniform_int_distribution<size_t> rnd_dist_{1, 50};
};

#endif  // ADRENALINE_MEDKITMANAGER_H
