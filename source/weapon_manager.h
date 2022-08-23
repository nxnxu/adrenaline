#ifndef ADRENALINE_WEAPON_MANAGER_H
#define ADRENALINE_WEAPON_MANAGER_H

#include <glm/glm.hpp>
#include <vector>

#include "PrimitiveManager.h"
#include "bullet.h"
#include "context.h"
#include "drawing_obj.h"
#include "weapon.h"

class WeaponManager {
    using vec3 = glm::vec3;

   public:
    WeaponManager() = default;

    template <typename T1, typename... T2>
    void add(T1 arg, T2... args) {
        weapons_.emplace_back(arg, args...);
    }

    void tick(Context const& context, std::vector<Bullet>& bullets,
              vec3 const& cam_pos, vec3 const& forward, vec3 const& right,
              DrawingObj& draw_obj);

   private:
    std::vector<Weapon::Weapon> weapons_;
    size_t index_{};
    size_t last_weapon_switch_time_{};
};

#endif  // ADRENALINE_WEAPON_MANAGER_H
