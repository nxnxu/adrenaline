#ifndef ADRENALINE_PLAY_VIEW_H
#define ADRENALINE_PLAY_VIEW_H

#include <glm/glm.hpp>

#include "AIManager.h"
#include "CameraManager.h"
#include "MedKitManager.h"
#include "MiniMap.h"
#include "ParticleManager.h"
#include "PrimitiveManager.h"
#include "ShaderManager.h"
#include "collision.h"
#include "context.h"
#include "crp_functions.h"
#include "ctv_functions.h"
#include "drawing_obj.h"
#include "input.h"
#include "maze.h"
#include "mouse_controller.h"
#include "player.h"
#include "view.h"
#include "weapon.h"
#include "weapon_manager.h"
#include "window.h"

struct PlayViewConfig {
    using vec3 = glm::vec3;

    FilePath m_path;
    MazeConfig m_map_config;
    PlayerConfig m_player_config;
    size_t m_max_particles;
    size_t m_max_primitives;
    AIConfig m_ai_config;
    size_t m_num_ais;
};

class PlayView : public View {
    using vec3 = glm::vec3;

   public:
    PlayView(Context& context, PlayViewConfig const& config);
    ViewTaskType tick(Context& context) override;
    void render(Context& context) override;

   private:
    void move_player_to_random_location() noexcept;

    void play(Context const& context);
    void process_player_bullets(Context const& context) noexcept;
    void process_ai_bullets(Context const& context) noexcept;

    void reset(Context const& context) noexcept;

    Maze maze_;

    DrawingObj drawing_obj_;

    CameraManager camera_manager_;
    ShaderManager shader_manager_;

    size_t primitive_shader_{};
    size_t maze_shader_{};
    size_t aimanager_shader_{};

    Player player_;
    WeaponManager weapon_manager_{};

    std::vector<Bullet> player_bullets_;
    std::vector<Bullet> ai_bullets_;

    MedKitManager med_kit_manager_;

    AIManager ai_manager_;

    WindowTitleMaker window_title_maker_{100};

    MiniMap minimap_;

    uint32_t play_begin_time_{};
    uint32_t play_last_time_{};

    PlayerConfig player_config_;
};

#endif  // ADRENALINE_PLAY_VIEW_H
