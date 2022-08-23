#ifndef ADRENALINE_VIEW_H
#define ADRENALINE_VIEW_H

#include <utility>

#include "context.h"
#include "cstdio"
#include "maze.h"

enum class ViewTaskType {
    CONTINUE,
    POP,
    PUSH_PLAY_VIEW,
    PUSH_HOME_VIEW,
};

struct FilePath {
    std::string m_shader_base_path;
    std::string m_assets_base_path;
};

class View {
   public:
    View() = default;

    virtual MazeConfig get_map_config() {
        throw std::logic_error{"get_map_size not allowed in View class"};
    }

    virtual ViewTaskType tick(Context& context) = 0;
    virtual void render(Context& context) = 0;
    virtual ~View() = default;
};

#endif  // ADRENALINE_VIEW_H
