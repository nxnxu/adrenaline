#include "AI.h"

using glm::mat4;
using glm::vec3;
using glm::vec4;

AI::AI(AIConfig const& config) noexcept
    : health_{config.m_initial_health},
      base_nozzle_origin{config.m_nozzle_origin},
      curr_nozzle_origin{base_nozzle_origin},
      forward_{config.m_forward},
      right_{glm::normalize(glm::cross(forward_, glm::vec3{0.0f, 1.0f, 0.0f}))}
      // up vector is always in +ve y
      ,
      aabb_{config.m_aabb},
      angular_speed_{config.m_angular_speed},
      speed_{config.m_speed},
      weapon_{config.m_weapon},
      space_radius_{config.m_space_radius} {}

void AI::tick(Context const& context, DrawingObj& dwo, Maze& maze,
              vec3 const& player_pos, vec3 const& eye_pos,
              std::vector<Bullet>& ai_bullets) {
    (void)dwo;
    (void)eye_pos;
    bool player_visible = can_see_player(context, maze, player_pos);

    vec3 ai_to_player = player_pos - pos_;

    if (player_visible) {
        // AI can see the player.
        // Set dest node to the location of player
        // and rst node to the location of player.
        // Now AI should travel towards the destination node.
        rotate_head_towards_player(context, ai_to_player);
        translation_task_.m_dst_node = maze.nearest_node(player_pos);
        translation_task_.m_rst_node = translation_task_.m_dst_node;

        bool allow_fire = (context.m_this_frame_time - weapon_.last_fire_time) >
                          weapon_.fire_delay();
        if (allow_fire) {
            ai_bullets.emplace_back(weapon_.fire(
                context, curr_nozzle_origin + pos_, forward_, right_));
            weapon_.last_fire_time = context.m_this_frame_time;
        }
    } else {
        // AI cannot see the player. So no need to stop anywhere just travel
        // towards destination.
        translation_task_.m_rst_node = std::numeric_limits<size_t>::max();
    }

    // If player is visible and far, go towards the player.
    // If player is not visible go to
    if (!player_visible || glm::length(ai_to_player) > space_radius_) {
        do_translation_job(context, maze);
    }
}

void AI::rotate_head_towards_player(Context const& context,
                                    glm::vec3 const& ai_to_player) noexcept {
    (void)context;

    float rotation_angle = 0.0f;
    if (!combinedToleranceCompare(glm::length(ai_to_player), 0.0f)) {
        auto ai_to_player_dir = glm::normalize(ai_to_player);
        forward_ = glm::normalize(forward_);
        rotation_angle =
            glm::degrees(vecxz_align_angle(forward_, ai_to_player_dir));
        float angular_speed_ms = angular_speed_ / 1000.0;
        float sign = rotation_angle < 0.0f ? -1.0f : 1.0f;
        rotation_angle = sign * std::min(std::fabs(rotation_angle),
                                         angular_speed_ms * context.m_dt);
    }

    auto rot_mat = glm::rotate(glm::mat4{1.0f}, glm::radians(rotation_angle),
                               glm::vec3{0.0f, 1.0f, 0.0f});
    forward_ = glm::normalize(rot_mat * vec4{forward_, 0.0f});
    right_ = glm::normalize(glm::cross(
        forward_,
        glm::vec3{0.0f, 1.0f, 0.0f}));  // up vector is always in +ve y
    head_rot_ = rot_mat * head_rot_;
    curr_nozzle_origin = head_rot_ * glm::vec4{base_nozzle_origin, 0.0f};
}

void AI::translate_to_node_center(size_t node, vec3 const& vec) noexcept {
    translation_task_.m_cur_node = translation_task_.m_nxt_node =
        translation_task_.m_dst_node = node;
    translate(vec);
}

void AI::translate(vec3 const& vec) noexcept {
    pos_ += vec;
    aabb_.translate(vec);
}

void AI::do_translation_job(Context const& context, Maze& maze) noexcept {
    // If AI has travelled all the distance in translation_task.m_dist
    if (combinedToleranceCompare(translation_task_.m_dist, 0.0f)) {
        // AI has reached the next node so current is now next node.
        translation_task_.m_cur_node = translation_task_.m_nxt_node;

        // If the current node is rst_node. STOP.
        // rst_node was the last location where the player was seen.
        // Now AI stands there and looks for player.
        // If the AI can see the player it will go towards it.
        if (translation_task_.m_cur_node == translation_task_.m_rst_node) {
            return;
        }

        // AI has reached its destination node.
        // Now find a new random destination node for the AI to go.
        if (translation_task_.m_cur_node == translation_task_.m_dst_node) {
            translation_task_.m_dst_node =
                maze.get_random_node(translation_task_.m_cur_node);
        }

        // Find the next node to visit such that AI can go to destination node
        // from current node
        translation_task_.m_nxt_node = maze.get_path_next_node(
            translation_task_.m_cur_node, translation_task_.m_dst_node);

        // If current node and next node are different then
        // set the direction and distance to travel to go to next node.
        if (translation_task_.m_cur_node != translation_task_.m_nxt_node) {
            auto a = maze.node_pos_center(translation_task_.m_cur_node);
            auto b = maze.node_pos_center(translation_task_.m_nxt_node);
            translation_task_.m_dir = glm::normalize(b - a);
            translation_task_.m_dist = glm::length(b - a);
        } else {
            // next node is same as current node. SKIP.
            translation_task_.m_dir = vec3{0.0f};
            translation_task_.m_dist = 0.0f;
        }
    }

    // Move AI in m_dir direction
    float sp_ms = speed_ / 1000.0f;
    float displace = std::min(translation_task_.m_dist, context.m_dt * sp_ms);
    translate(translation_task_.m_dir * displace);
    translation_task_.m_dist -= displace;
}

bool AI::can_see_player(Context const& context, Maze& maze,
                        vec3 const& player_pos) noexcept {
    (void)context;
    /*check if AI's view sight is being obstructed by any obstacle*/
    auto result = maze_line_collision_test(pos_, player_pos, maze);
    return !result.m_is_intersecting;
}