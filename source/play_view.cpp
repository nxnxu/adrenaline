#include "play_view.h"

using glm::mat4;
using glm::vec3;
using glm::vec4;

/*
 * TODO: add collision dection and resolution betweeen player and AI's
 * */

PlayView::PlayView(Context& context, PlayViewConfig const& config)
    : maze_{config.m_map_config},
      drawing_obj_{config.m_max_primitives},
      camera_manager_{},
      shader_manager_{config.m_path.m_shader_base_path},
      player_{context, config.m_player_config, camera_manager_},
      med_kit_manager_{
          context,
          maze_,
      },
      ai_manager_{config.m_ai_config, config.m_num_ais, maze_},
      minimap_{1000000, 0.0f, maze_.extreme_x(), 0.0f, maze_.extreme_z()},
      play_begin_time_{context.m_this_frame_time},
      play_last_time_{context.m_this_frame_time},
      player_config_{config.m_player_config} {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /* setup shader */

    primitive_shader_ = shader_manager_.add("primitive");
    maze_shader_ = shader_manager_.add("maze");
    aimanager_shader_ = shader_manager_.add("weapon");

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    move_player_to_random_location();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*setup up player weapons*/

    weapon_manager_.add(Weapon::Weapon{10, 100, 50, 20, DOT_CROSSHAIR,
                                       BULLET_WL45_TICK, BULLET_WL45_HIT});
    weapon_manager_.add(Weapon::Weapon{80, 50, 50, 50, SQUARE_CROSSHAIR,
                                       BULLET_EB95_TICK, BULLET_EB95_HIT});

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

ViewTaskType PlayView::tick(Context& context) {
    auto& input = *context.m_input;

    if (player_.health()) {
        play(context);
    } else if (input.is_event_occured(context.m_actions.m_restart_game)) {
        // restart the game, by reseting everything
        reset(context);
    }

    if (input.is_event_occured(context.m_actions.m_quit_game)) {
        return ViewTaskType::POP;
    }

    return ViewTaskType::CONTINUE;
}

void PlayView::reset(Context const& context) noexcept {
    play_begin_time_ = context.m_this_frame_time;
    player_.add_health(player_config_.m_initial_health);
    move_player_to_random_location();
    ai_manager_.reset(context, maze_);
    med_kit_manager_.reset(context, maze_);
    player_bullets_.clear();
    ai_bullets_.clear();
    drawing_obj_.particle_manager_.reset();
}

void PlayView::play(Context const& context) {
    camera_manager_.set(player_.cam_id());

    player_.tick(context, maze_, camera_manager_);

    size_t collected_health = med_kit_manager_.tick(
        context, maze_, drawing_obj_.particle_manager_, player_.position());

    player_.add_health(collected_health);

    weapon_manager_.tick(context, player_bullets_, camera_manager_.position(),
                         camera_manager_.forward_axis(),
                         camera_manager_.right_axis(), drawing_obj_);

    ai_manager_.tick(context, drawing_obj_, maze_, player_.position(),
                     camera_manager_.position(), ai_bullets_);

    process_player_bullets(context);
    process_ai_bullets(context);

    ai_manager_.mark_ai_pos_minimap(context, minimap_);
    med_kit_manager_.mark_medkit_in_minimap(context, minimap_);

    minimap_.add_temporary(5, player_.position(),
                           glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});

    minimap_.tick(context);

    camera_manager_.tick(context);
    drawing_obj_.tick(context);

    play_last_time_ = context.m_this_frame_time;
}

void PlayView::process_player_bullets(Context const& context) noexcept {
    size_t i = 0;
    size_t j = player_bullets_.size();

    while (i < j) {
        player_bullets_[i].m_tick(context, player_bullets_[i],
                                  drawing_obj_.particle_manager_);

        float travelled_dist = glm::length(player_bullets_[i].m_birth_pos -
                                           player_bullets_[i].m_current_pos);

        auto query =
            maze_line_collision_test(player_bullets_[i].m_old_pos,
                                     player_bullets_[i].m_current_pos, maze_);

        /*check if bullet hits any AI's, if yes find first AI, which gets hit*/
        AI* hit_ai = nullptr;
        float nearest_dist = std::numeric_limits<float>::max();
        vec3 nearest_ai_hit_pos;

        for (auto& ai : ai_manager_.ais()) {
            vec3 contact;
            bool hit = line_aabb_intersection(player_bullets_[i].m_old_pos,
                                              player_bullets_[i].m_current_pos,
                                              ai.aabb(), contact);

            if (hit) {
                float d = glm::length(contact - player_bullets_[i].m_old_pos);
                if (d < nearest_dist) {
                    hit_ai = &ai;
                    nearest_dist = d;
                    nearest_ai_hit_pos = contact;
                }
            }
        }

        if (hit_ai != nullptr || query.m_is_intersecting) {
            vec3 bullet_hit_pos;

            /*player bullet hits both ai and maze*/
            if (hit_ai != nullptr && query.m_is_intersecting) {
                /*hits ai first*/
                if (!nearest(player_bullets_[i].m_old_pos, nearest_ai_hit_pos,
                             query.m_intersecting_point, bullet_hit_pos)) {
                    hit_ai->bullet_hit(player_bullets_[i].m_damage);
                    /*AI is dead, spawn a medkit in its position*/
                    if (!hit_ai->health()) {
                        med_kit_manager_.add_med_kit(context.m_this_frame_time,
                                                     maze_, 50,
                                                     hit_ai->position());
                    }
                }
            }
            /*player bullet hits only AI*/
            else if (hit_ai != nullptr) {
                bullet_hit_pos = nearest_ai_hit_pos;
                hit_ai->bullet_hit(player_bullets_[i].m_damage);
                /*AI is dead, spawn a medkit in its position*/
                if (!hit_ai->health()) {
                    med_kit_manager_.add_med_kit(context.m_this_frame_time,
                                                 maze_, 50, hit_ai->position());
                }
            }

            /*player bullet hits only maze*/
            else if (query.m_is_intersecting) {
                bullet_hit_pos = query.m_intersecting_point;
            }

            player_bullets_[i].m_hit(context, player_bullets_[i],
                                     bullet_hit_pos,
                                     drawing_obj_.particle_manager_);
            /*discard this bullet, by replace it with another one*/
            player_bullets_[i] = player_bullets_[j - 1];
            --j;
        }
        /*player bullet has travelled out of range*/
        else if (travelled_dist > player_bullets_[i].m_range) {
            /*discard this bullet, by replace it with another one*/
            player_bullets_[i] = player_bullets_[j - 1];
            --j;

        } else {
            ++i;
        }
    }

    player_bullets_.erase(player_bullets_.begin() + j, player_bullets_.end());
}

void PlayView::process_ai_bullets(Context const& context) noexcept {
    size_t i = 0;
    size_t j = ai_bullets_.size();

    while (i < j) {
        ai_bullets_[i].m_tick(context, ai_bullets_[i],
                              drawing_obj_.particle_manager_);
        float travelled_dist = glm::length(ai_bullets_[i].m_birth_pos -
                                           ai_bullets_[i].m_current_pos);

        /*check if bullet hits player*/
        vec3 player_hit_pos;
        bool player_hit = line_aabb_intersection(
            ai_bullets_[i].m_old_pos, ai_bullets_[i].m_current_pos,
            player_.aabb(), player_hit_pos);

        /*check if bullet hits maze*/
        auto query = maze_line_collision_test(
            ai_bullets_[i].m_old_pos, ai_bullets_[i].m_current_pos, maze_);

        if (player_hit || query.m_is_intersecting) {
            vec3 bullet_hit_pos;

            /*bullet is hitting both, find first hit point*/
            if (player_hit && query.m_is_intersecting) {
                /*player got hit first*/
                if (!nearest(ai_bullets_[i].m_old_pos, player_hit_pos,
                             query.m_intersecting_point, bullet_hit_pos)) {
                    player_.bullet_hit(ai_bullets_[i].m_damage);
                }
            } else if (player_hit) {
                bullet_hit_pos = player_hit_pos;
                player_.bullet_hit(ai_bullets_[i].m_damage);
            } else if (query.m_is_intersecting) {
                bullet_hit_pos = query.m_intersecting_point;
            }
            ai_bullets_[i].m_hit(context, ai_bullets_[i], bullet_hit_pos,
                                 drawing_obj_.particle_manager_);
            ai_bullets_[i] = ai_bullets_[j - 1];
            --j;
        }
        /*bullet has travelled out of its hit range, discard it*/
        else if (travelled_dist > ai_bullets_[i].m_range) {
            ai_bullets_[i] = ai_bullets_[j - 1];
            --j;
        } else {
            ++i;
        }
    }
    ai_bullets_.erase(ai_bullets_.begin() + j, ai_bullets_.end());
}

void PlayView::render(Context& context) {
    glViewport(0, 0, context.m_window_width, context.m_window_height);

    static GLfloat black_color[] = {0.0f, 0.0f, 0.0f, 1.0f};

    GLfloat depth = 1.0f;
    glEnable(GL_DEPTH_TEST);
    glClearBufferfv(GL_COLOR, 0, black_color);
    glClearBufferfv(GL_DEPTH, 0, &depth);
    glDisable(GL_BLEND);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    camera_manager_.set(player_.cam_id());
    context.m_view = camera_manager_.view();
    context.m_proj = camera_manager_.p_proj();
    context.m_vp = context.m_proj * context.m_view;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*Render stuff requiring DEPTH TEST*/

    /* Render MazeInternal */

    shader_manager_.use(maze_shader_);
    shader_manager_.set("vp", context.m_vp);
    maze_.render(context, shader_manager_.shader());
    maze_.render_path(context, shader_manager_.shader());

    /*Render AI*/

    shader_manager_.use(aimanager_shader_);
    shader_manager_.set("vp", context.m_vp);
    ai_manager_.render(context, shader_manager_.shader(), glm::mat4{1.0f});

    /* Render primitives*/

    shader_manager_.use(primitive_shader_);
    shader_manager_.set("vp", context.m_vp);
    shader_manager_.set("p_proj", context.m_proj);

    drawing_obj_.lm_.render(context);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    drawing_obj_.pm_.render(context);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*Render stuff requiring no DEPTH TEST*/

    /*render primitives*/

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    drawing_obj_.no_depth_pm_.render(context);
    drawing_obj_.no_depth_lm_.render(context);

    /*render mini-map*/

    /* set new view port, then render minimap in it*/
    size_t origin_x = context.m_window_width * 75 / 100;  // 75 percent from x

    size_t width = context.m_window_width * 20 / 100;  // 20 percent width
    size_t height = width;                             // 20 percent height

    size_t origin_y = context.m_window_height -
                      (width + context.m_window_height * 10 /
                                   100);  // 30 percent of y, and flip it

    glViewport(origin_x, origin_y, width, height);

    minimap_.m_no_depth_pm.render(context);
    minimap_.m_no_depth_lm.render(context);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*notify current status, via title bar*/

    window_title_maker_.clean();

    uint32_t hour, minute, seconds;

    uint32_t milli = (play_last_time_ - play_begin_time_) / 1000;
    calculate_time(milli, hour, minute, seconds);

    if (!player_.health()) {
        window_title_maker_.add("[ ", hour, 2);
        window_title_maker_.add("", minute, 2);
        window_title_maker_.add("", seconds, 2);
        window_title_maker_.add("]");
        window_title_maker_.add(
            "YOU ARE DEAD, PRESS ESC, OR RETURN TO RESTART GAME");
    } else {
        window_title_maker_.add("USE W/A/S/D/F/ESC");
        window_title_maker_.add("FPS: ", context.m_fps, 5);
        window_title_maker_.add("HEALTH: ", player_.health(), 9);
        window_title_maker_.add("[ ", hour, 2);
        window_title_maker_.add("", minute, 2);
        window_title_maker_.add("", seconds, 2);
        window_title_maker_.add("]");
    }
    context.m_window->set_title(window_title_maker_.buffer());
}

void PlayView::move_player_to_random_location() noexcept {
    auto node = maze_.get_random_node();
    auto node_pos = maze_.node_pos_center(node);
    auto tv = node_pos - player_.position();
    player_.translate_no_collision_checking(tv, camera_manager_);
}