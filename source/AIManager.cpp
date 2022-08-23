#include "AIManager.h"

using glm::mat4;
using glm::vec3;
using glm::vec4;

AIManager::AIManager(AIConfig const& config, size_t num_ais, Maze& maze)
    : ai_config_{config}, ai_model_{config.m_ai_model} {
    for (size_t i = 0; i < num_ais; ++i) {
        ais_.emplace_back(spawn_ai(maze));
    }
}

void AIManager::render(Context const& context, Shader& shader,
                       glm::mat4 const& p_trans) noexcept {
    for (auto& ai : ais_) {
        ai_model_->render(context, shader, ai.pos_, ai.head_rot_, p_trans);
    }
}

void AIManager::tick(Context const& context, DrawingObj& dwo, Maze& maze,
                     vec3 const& player_pos, vec3 const& eye_pos,
                     std::vector<Bullet>& ai_bullets) {
    for (auto& ai : ais_) {
        ai.tick(context, dwo, maze, player_pos, eye_pos, ai_bullets);
    }

    /*respawn AI if its health is zero*/
    for (auto& ai : ais_) {
        if (!ai.health()) {
            ai = spawn_ai(maze);
        }
    }
}

AI AIManager::spawn_ai(Maze& maze) noexcept {
    AI ai{ai_config_};
    /*Now translate the ai, in to some random location*/
    auto node = maze.get_random_node();
    auto node_pos = maze.node_pos_center(node);
    ai.translate_to_node_center(node, node_pos);
    return ai;
}

void AIManager::mark_ai_pos_minimap(Context const& context,
                                    MiniMap& minimap) noexcept {
    (void)context;
    for (auto const& ai : ais_) {
        minimap.add_temporary(5, ai.position(),
                              glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
    }
}

void AIManager::reset(Context const& context, Maze& maze) noexcept {
    (void)context;
    for (auto& ai : ais_) {
        ai = spawn_ai(maze);
    }
}