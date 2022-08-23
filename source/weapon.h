#ifndef ADRENALINE_WEAPON_FUNCTIONS_H
#define ADRENALINE_WEAPON_FUNCTIONS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

#include "PrimitiveManager.h"
#include "bullet.h"
#include "bullet_functions.h"
#include "context.h"
#include "drawing_obj.h"
#include "helper.h"

void SQUARE_CROSSHAIR(Context const &context, DrawingObj &draw_obj);
void DOT_CROSSHAIR(Context const &context, DrawingObj &draw_obj);

namespace Weapon {
using fire_t =
    std::function<Bullet(Context const &, glm::vec3 const &origin,
                         glm::vec3 const &forward, glm::vec3 const &right)>;
using crosshair_t =
    std::function<void(Context const &context, DrawingObj &draw_obj)>;
using bullet_tick_t = std::function<void(Context const &context, Bullet &bullet,
                                         ParticleManager &pm)>;
using bullet_hit_t =
    std::function<void(Context const &context, Bullet &bullet,
                       glm::vec3 const &hit, ParticleManager &pm)>;

class Weapon {
   public:
    Weapon(uint32_t fire_delay, float bullet_speed, float bullet_range,
           size_t bullet_damage, crosshair_t crosshair,
           bullet_tick_t bullet_tick, bullet_hit_t bullet_hit)
        : fire_delay_{fire_delay},
          bullet_speed_{bullet_speed},
          bullet_range_{bullet_range},
          bullet_damage_{bullet_damage},
          generate_crosshair_{std::move(crosshair)},
          bullet_tick_{std::move(bullet_tick)},
          bullet_hit_{std::move(bullet_hit)} {}

    Bullet fire(Context const &context, glm::vec3 const &origin,
                glm::vec3 const &forward, glm::vec3 const &right) {
        (void)context;
        (void)right;
        glm::vec3 velocity = forward * bullet_speed_;
        return Bullet{origin,   bullet_damage_, bullet_range_,
                      velocity, bullet_tick_,   bullet_hit_};
    }

    void generate_crosshair(Context const &context, DrawingObj &draw_obj) {
        generate_crosshair_(context, draw_obj);
    }

    size_t damage() const noexcept { return bullet_damage_; }
    size_t fire_delay() const noexcept { return fire_delay_; }
    size_t last_fire_time{};

   private:
    uint32_t fire_delay_;
    float bullet_speed_;
    float bullet_range_;
    size_t bullet_damage_;
    crosshair_t generate_crosshair_;
    bullet_tick_t bullet_tick_;
    bullet_hit_t bullet_hit_;
};

}  // namespace Weapon

#endif  // ADRENALINE_WEAPON_FUNCTIONS_H
