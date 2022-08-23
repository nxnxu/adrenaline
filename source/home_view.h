#ifndef ADRENALINE_HOME_VIEW_H
#define ADRENALINE_HOME_VIEW_H

#include <glm/glm.hpp>
#include <memory>

#include "CameraManager.h"
#include "ParticleManager.h"
#include "PrimitiveManager.h"
#include "ShaderManager.h"
#include "context.h"
#include "crp_functions.h"
#include "ctv_functions.h"
#include "helper.h"
#include "input.h"
#include "maze.h"
#include "mouse_controller.h"
#include "player.h"
#include "view.h"
#include "window.h"

struct HomeViewConfig {
    FilePath m_path;
    MazeConfig m_map_config; /*min config a map can have*/
};

class HomeView : public View {
   public:
    HomeView(Context& context, HomeViewConfig const& config);

    HomeView(HomeView const&) = delete;

    MazeConfig get_map_config() override;

    ViewTaskType tick(Context& context) override;
    void render(Context& context) override;

   private:
    MazeConfig min_map_config_;

    /*
     * users choices are stores in sizes_
     * sizes_: represents PN, PM of maze, number of nodes per maze
     * */

    size_t sizes_{};

    std::unique_ptr<Maze> maze_;

    CameraManager camera_manager_{};
    ShaderManager shader_manager_;

    size_t monitor_cam_id{};
    size_t maze_shader_{};

    WindowTitleMaker window_title_maker_{1000};
};

#endif  // ADRENALINE_HOME_VIEW_H
