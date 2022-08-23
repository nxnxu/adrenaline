#include "weapon_manager.h"

using glm::vec3;

void WeaponManager::tick(Context const& context, std::vector<Bullet>& bullets,
                         vec3 const& cam_pos, vec3 const& forward,
                         vec3 const& right, DrawingObj& draw_obj) {
    if (weapons_.empty()) {
        return;
    }
    Input& input = *context.m_input;

    bool allow_weapon_switch =
        (context.m_this_frame_time - last_weapon_switch_time_) >
        context.m_weapon_switch_dt;

    if (allow_weapon_switch &&
        input.is_event_occured(context.m_actions.m_next_weapon)) {
        index_ = std::min(index_ + 1, weapons_.size() - 1);
        last_weapon_switch_time_ = context.m_this_frame_time;
    }
    if (allow_weapon_switch &&
        input.is_event_occured(context.m_actions.m_prev_weapon)) {
        index_ = index_ ? index_ - 1 : index_;
        last_weapon_switch_time_ = context.m_this_frame_time;
    }

    bool allow_fire =
        (context.m_this_frame_time - weapons_[index_].last_fire_time) >
        weapons_[index_].fire_delay();

    if (allow_fire && input.is_event_occured(context.m_actions.m_fire)) {
        bullets.emplace_back(
            weapons_[index_].fire(context, cam_pos, forward, right));
        weapons_[index_].last_fire_time = context.m_this_frame_time;
    }

    weapons_[index_].generate_crosshair(context, draw_obj);
}
