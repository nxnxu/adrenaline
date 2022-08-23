#include "home_view.h"

using glm::mat4;
using glm::vec3;
using glm::vec4;

HomeView::HomeView(Context &context, HomeViewConfig const &config)
    : min_map_config_{config.m_map_config},
      sizes_{min_map_config_.PN},
      maze_{new Maze{min_map_config_}},
      shader_manager_{config.m_path.m_shader_base_path} {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /* set up home camera */

    float aspect =
        (1.0f * context.m_window_width) / (1.0f * context.m_window_height);
    float fov = 45.0f;
    float z_near = 0.01;
    float z_far = 1000.0f;

    vec3 home_cam_loc = vec3(0.0f, 0.0f, 0.0f);
    vec3 home_cam_tar = vec3{0.0f, -1.0f, 0.0f};
    vec3 home_cam_upx = vec3(0.0f, 0.0f, -1.0f);

    CameraConfig home_camera_config{
        fov,          aspect,       z_near, z_far,    home_cam_loc,
        home_cam_tar, home_cam_upx, false,  crp_G_XY, ctv_G_XY};

    monitor_cam_id = camera_manager_.add(context, home_camera_config);
    camera_manager_.set(monitor_cam_id);

    /*move the camera to center of map and raise it by some height*/
    auto tv = glm::vec3{maze_->extreme_x() / 2.0f, maze_->extreme_x(),
                        maze_->extreme_z() / 2.0f};
    camera_manager_.translate(tv);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    maze_shader_ = shader_manager_.add("maze");
}

ViewTaskType HomeView::tick(Context &context) {
    auto &input = *context.m_input;

    camera_manager_.tick(context);

    bool value_change = false;

    if (input.is_event_occured(context.m_actions.m_home_view_increase)) {
        sizes_ += 1;
        value_change = true;
    }

    if (input.is_event_occured(context.m_actions.m_home_view_decrease)) {
        sizes_ = std::max(sizes_ - 1, min_map_config_.PN);
        value_change = true;
    }

    if (value_change) {
        MazeConfig map_config = get_map_config();
        maze_.reset(new Maze{map_config});

        /*reposition the camera by moving, the camera to center of map and raise
         * it by some height*/

        auto cam_pos = camera_manager_.position();

        cam_pos.x = maze_->extreme_x() / 2.0f;
        cam_pos.y = maze_->extreme_x();
        cam_pos.z = maze_->extreme_x() / 2.0f;

        camera_manager_.translate(cam_pos - camera_manager_.position());
    }

    if (input.is_event_occured(context.m_actions.m_home_view_play)) {
        return ViewTaskType::PUSH_PLAY_VIEW;
    }

    if (input.is_event_occured(context.m_actions.m_quit_app)) {
        return ViewTaskType::POP;
    }

    return ViewTaskType::CONTINUE;
}

void HomeView::render(Context &context) {
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
    /*set up camera matrices*/

    context.m_view = camera_manager_.view();
    context.m_proj = camera_manager_.p_proj();
    context.m_vp = context.m_proj * context.m_view;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /* Render MazeInternal */

    shader_manager_.use(maze_shader_);
    shader_manager_.set("vp", context.m_vp);
    maze_->render(context, shader_manager_.shader());

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*notify current status, via title bar*/

    uint32_t milli = context.m_this_frame_time / 1000;
    uint32_t hour, minute, seconds;

    calculate_time(milli, hour, minute, seconds);

    window_title_maker_.clean();
    window_title_maker_.add("USE I/D/ENTER/ESC");

    window_title_maker_.add("[FPS ", context.m_fps, 5);
    window_title_maker_.add("]", 0, 0);

    window_title_maker_.add("MAP SIZE: ", sizes_, 5);

    window_title_maker_.add("[ ", hour, 2);
    window_title_maker_.add("", minute, 2);
    window_title_maker_.add("", seconds, 2);
    window_title_maker_.add("]");

    context.m_window->set_title(window_title_maker_.buffer());
}

MazeConfig HomeView::get_map_config() {
    MazeConfig map_config = min_map_config_;

    map_config.PN = sizes_;
    map_config.PM = sizes_;

    return map_config;
}