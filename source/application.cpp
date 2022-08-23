#include "application.h"

#include <iostream>
#include <numeric>
#include <stack>

#include "AIManager.h"
#include "CameraManager.h"
#include "ParticleManager.h"
#include "PrimitiveManager.h"
#include "ShaderManager.h"
#include "camera.h"
#include "collision.h"
#include "context.h"
#include "crp_functions.h"
#include "ctv_functions.h"
#include "helper.h"
#include "home_view.h"
#include "maze.h"
#include "memory"
#include "mouse_controller.h"
#include "particle_tick_functions.h"
#include "play_view.h"
#include "player.h"
#include "random_generator.h"

using glm::mat4;
using glm::vec3;
using glm::vec4;

/*
 *
 * TODO: This file is mess, clean up
 * TODO: move some hardcoded values to external assest file
 * */

void Application::main(int argc, char **argv) {
    if (argc < 2 || argc > 2) {
        fprintf(
            stderr,
            "[error] launch program using: program_name base_directory_path\n");
        return;
    }

    std::string base_path = argv[1];

    FilePath path{
        .m_shader_base_path = base_path + get_path_separator() + "shader",
        .m_assets_base_path = base_path + get_path_separator() + "assets"};

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*
     * convention used
     * file named scene, will hold player and AI model files
     * */

    std::string scene_file_name = "scene";

    Importor::Importor import{path.m_assets_base_path, scene_file_name};

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Window window{0, 0, "adrenaline", true, true, false};
    Input input{};
    Context context{};
    MouseController mouse{};

    context.m_window = &window;
    context.m_input = &input;
    context.m_mouse_controller = &mouse;
    context.m_mouse_sensitivity = 1000;

    context.m_actions.m_home_view_increase =
        Event{.m_type = EventType::KEYUP, .m_key = KEY::I};
    context.m_actions.m_home_view_decrease =
        Event{.m_type = EventType::KEYUP, .m_key = KEY::D};
    context.m_actions.m_home_view_play =
        Event{.m_type = EventType::KEYUP, .m_key = KEY::RETURN};

    context.m_actions.m_move_forward =
        Event{.m_type = EventType::KEYPRESS, .m_key = KEY::W};
    context.m_actions.m_move_backward =
        Event{.m_type = EventType::KEYPRESS, .m_key = KEY::S};
    context.m_actions.m_move_left =
        Event{.m_type = EventType::KEYPRESS, .m_key = KEY::A};
    context.m_actions.m_move_right =
        Event{.m_type = EventType::KEYPRESS, .m_key = KEY::D};

    context.m_actions.m_quit_app =
        Event{.m_type = EventType::KEYUP, .m_key = KEY::ESCAPE};
    context.m_actions.m_quit_game =
        Event{.m_type = EventType::KEYUP, .m_key = KEY::ESCAPE};

    context.m_actions.m_fire = {EventType::KEYPRESS, .m_key = KEY::F};
    context.m_actions.m_next_weapon = {EventType::MOUSEWHEEL,
                                       .m_mouse = Mouse::WHEELDOWN};
    context.m_actions.m_prev_weapon = {EventType::MOUSEWHEEL,
                                       .m_mouse = Mouse::WHEELUP};
    context.m_weapon_switch_dt = 100;

    context.m_actions.m_restart_game =
        Event{.m_type = EventType::KEYUP, .m_key = KEY::RETURN};

    input.set_relative_mouse_mode(true);
    context.tick();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::stack<std::unique_ptr<View> > views;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*set up AI config*/

    /*
     * Convention used
     * Scene file consists of a Model named AI
     * Its head position is marked with MODEL LOC ACTION named head,
     * its nozzle is marked with MODEL LOC ACTION named nozzle.
     * AI head which is a mpart, is named head
     *
     *
     * */

    std::string ai_name = "AI";
    std::string ai_head_lm_name = "head";
    std::string ai_nozzle_lm_name = "nozzle";
    std::string ai_head_mpart_name = "head";

    vec3 head_pos = std::move(import.m_scene)
                        .get_model(ai_name)
                        .m_header.get_loc_action(ai_head_lm_name)
                        .m_loc;
    vec3 nozzle_pos = std::move(import.m_scene)
                          .get_model(ai_name)
                          .m_header.get_loc_action(ai_nozzle_lm_name)
                          .m_loc;
    vec3 ai_forward = nozzle_pos - head_pos;
    AABB ai_aabb = calculate_aabb(import.m_scene.get_model(ai_name));

    Weapon::Weapon ai_weapon{
        100, 20, 50, 5, DOT_CROSSHAIR, BULLET_RL96_TICK, BULLET_RL96_HIT};

    AIModel ai_model{std::move(import.m_scene.get_model(ai_name)),
                     ai_head_mpart_name};
    AIConfig ai_config{100,        1.0f * 360.0f, 10.0f,
                       nozzle_pos, ai_forward,    ai_aabb,
                       &ai_model,  ai_weapon,     2.0f};

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*setup players configuration*/

    std::string player_model_name = "PLAYER";
    std::string camera_pos_lm_name = "CAM_POS";

    float aspect =
        (1.0f * context.m_window_width) / (1.0f * context.m_window_height);
    float fov = 45.0f;
    float z_near = 0.01;
    float z_far = 1000.0f;

    vec3 fps_cam_loc = import.m_scene.get_model(player_model_name)
                           .m_header.get_loc_action(camera_pos_lm_name)
                           .m_loc;
    vec3 fps_cam_tar = vec3{0.0f, 0.0f, -1.0f};
    vec3 fps_cam_upx = vec3(0.0f, 1.0f, 0.0f);

    CameraConfig player_cam_config{fov,         aspect,      z_near,      z_far,
                                   fps_cam_loc, fps_cam_tar, fps_cam_upx, false,
                                   crp_G_XY,    ctv_G_Y};

    PlayerConfig player_config{
        std::numeric_limits<int>::max(),
        calculate_aabb(import.m_scene.get_model(player_model_name)),
        calculate_aabbxz(import.m_scene.get_model(player_model_name)),
        2.0f,
        15.0f,
        player_cam_config};

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /* setup play view configuration */

    PlayViewConfig play_view_config{
        path, {}, player_config, 1000000, 1000000, ai_config, 1};

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /* setup home view config */
    /*minimum map config*/
    MazeConfig min_map_config{1, 1, 4, 4, 2, 2, vec3{1.0f}, vec3{0.0f}};

    views.emplace(new HomeView{context, HomeViewConfig{path, min_map_config}});

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /* Application Loop */

    while (!input.m_quit_event && !views.empty()) {
        window.tick();
        context.tick();
        input.tick();
        mouse.tick(context);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ViewTaskType response = views.top()->tick(context);

        switch (response) {
            case ViewTaskType::POP:
                views.pop();
                break;
            case ViewTaskType::CONTINUE:
                break;
            case ViewTaskType::PUSH_PLAY_VIEW:
                play_view_config.m_map_config = views.top()->get_map_config();
                {
                    int size = play_view_config.m_map_config.PN;
                    play_view_config.m_num_ais =
                        std::max(1, size * size / 5 + 1);
                }
                views.emplace(new PlayView{context, play_view_config});
                break;
            default:
                assert(0);
        }

        if (!views.empty()) {
            views.top()->render(context);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        window.swap_buffers();
    }
}
