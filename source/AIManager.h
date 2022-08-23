#ifndef ADRENALINE_AIMANAGER_H
#define ADRENALINE_AIMANAGER_H

#include <vector>

#include "AI.h"
#include "AIModel.h"
#include "MiniMap.h"
#include "bullet.h"
#include "bullet_functions.h"
#include "drawing_obj.h"
#include "maze.h"

class AIManager {
    using vec3 = glm::vec3;
    using vec4 = glm::vec4;
    using mat4 = glm::mat4;

   public:
    explicit AIManager(AIConfig const& config, size_t num_ais, Maze& maze);

    void tick(Context const& context, DrawingObj& dwo, Maze& maze,
              vec3 const& player_pos, vec3 const& eye_pos,
              std::vector<Bullet>& ai_bullets);

    void render(Context const& context, Shader&, mat4 const& p_trans) noexcept;
    std::vector<AI>& ais() noexcept { return ais_; };
    void mark_ai_pos_minimap(Context const& context, MiniMap& minimap) noexcept;

    void reset(Context const& context, Maze& maze) noexcept;

   private:
    AI spawn_ai(Maze& maze) noexcept;

    AIConfig ai_config_;
    AIModel* ai_model_;
    std::vector<AI> ais_;
};

#endif  // ADRENALINE_AIMANAGER_H
