#include "player.h"

Player::Player(Context const& context, PlayerConfig const& config,
               CameraManager& cm)
    : health_{config.m_initial_health},
      aabb_{config.m_aabb},
      aabbxz_{config.m_aabbxz},
      walk_speed_{config.m_walk_speed},
      run_speed_{config.m_run_speed} {
    cam_id_ = cm.add(context, config.camera_config);
}

void Player::tick(Context const& context, Maze& maze, CameraManager& cm) {
    cm.set(cam_id_);
    process_player_movement(context, maze, cm);
}

void Player::process_player_movement(Context const& context, Maze& maze,
                                     CameraManager& cm) {
    auto& input = *context.m_input;
    auto& mouse = *context.m_mouse_controller;

    cm.rotate(context, mouse.get_x_dt(), mouse.get_y_dt(), 0.0f);

    float speed = (run_speed_ / 1000.0f) * context.m_dt;
    vec3 tv{0.0f};

    if (input.is_event_occured(context.m_actions.m_move_forward)) {
        tv += cm.forward() * speed;
    }

    if (input.is_event_occured(context.m_actions.m_move_backward)) {
        tv += cm.backward() * speed;
    }

    if (input.is_event_occured(context.m_actions.m_move_left)) {
        tv += cm.left() * speed;
    }

    if (input.is_event_occured(context.m_actions.m_move_right)) {
        tv += cm.right() * speed;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /* now find the correct translation, such that player does not collides*/

    /*creating aabbxzmtv query*/
    AABBXZMTV aabbxzmtv;
    aabbxzmtv.m_aabb = aabbxz_;
    aabbxzmtv.m_xMove = !combinedToleranceCompare(tv.x, 0.0f);
    aabbxzmtv.m_zMove = !combinedToleranceCompare(tv.z, 0.0f);
    aabbxzmtv.m_xPositiveAxis = tv.x >= 0.0f;
    aabbxzmtv.m_zPositiveAxis = tv.z >= 0.0f;
    aabbxzmtv.m_x_dt = std::fabs(tv.x);
    aabbxzmtv.m_z_dt = std::fabs(tv.z);

    bool check_collision = !combinedToleranceCompare(glm::length(tv), 0.0f);
    size_t max_iterations = check_collision ? 5 : 0;

    maze_collision_resolution_aabbxzmtv(maze, aabbxzmtv, max_iterations,
                                        0.0001f, 0.000001f);

    vec3 rectified_x =
        vec3{aabbxzmtv.m_xPositiveAxis ? 1.0f : -1.0f, 0.0f, 0.0f} *
        std::fabs(aabbxzmtv.m_x_dt);
    vec3 rectified_z =
        vec3{0.0f, 0.0f, aabbxzmtv.m_zPositiveAxis ? 1.0f : -1.0f} *
        std::fabs(aabbxzmtv.m_z_dt);
    vec3 rectified = rectified_x + rectified_z;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*now translate player and camera by rectified vector*/
    translate_no_collision_checking(rectified, cm);
}

void Player::translate_no_collision_checking(vec3 const& vec,
                                             CameraManager& cm) noexcept {
    cm.set(cam_id_);
    aabbxz_.translate(vec);
    aabb_.translate(vec);
    position_ += vec;
    cm.translate(vec);
}