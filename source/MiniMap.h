#ifndef ADRENALINE_MINIMAP_H
#define ADRENALINE_MINIMAP_H

#include <glm/glm.hpp>
#include <vector>

#include "PrimitiveManager.h"
#include "context.h"
#include "helper.h"

class MiniMap {
   public:
    PrimitiveManager m_no_depth_pm;
    PrimitiveManager m_no_depth_lm;

    MiniMap(size_t max_primitives, float min_x, float max_x, float min_z,
            float max_z);
    void add_temporary(size_t point_size, glm::vec3 const& pos,
                       glm::vec4 const& color) noexcept;
    void tick(Context const& context) noexcept;

   private:
    float min_x_;
    float max_x_;
    float min_z_;
    float max_z_;
};

#endif  // ADRENALINE_MINIMAP_H
