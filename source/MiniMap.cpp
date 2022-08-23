#include "MiniMap.h"

MiniMap::MiniMap(size_t max_primitives, float min_x, float max_x, float min_z,
                 float max_z)
    : m_no_depth_pm{max_primitives, GL_POINTS},
      m_no_depth_lm{4, GL_LINES},
      min_x_{min_x},
      max_x_{max_x},
      min_z_{min_z},
      max_z_{max_z} {
    /*map edges*/

    std::vector<PrimitiveManager::Vertex> vertices;

    float offset = 0.8f;

    PrimitiveManager::Vertex l1{true, 1, glm::vec3{-offset, offset, 0.0f},
                                glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}};
    PrimitiveManager::Vertex l2{true, 1, glm::vec3{offset, offset, 0.0f},
                                glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}};

    PrimitiveManager::Vertex l3{true, 1, glm::vec3{-offset, -offset, 0.0f},
                                glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}};
    PrimitiveManager::Vertex l4{true, 1, glm::vec3{offset, -offset, 0.0f},
                                glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}};

    vertices.emplace_back(l1);
    vertices.emplace_back(l2);
    vertices.emplace_back(l2);
    vertices.emplace_back(l4);
    vertices.emplace_back(l4);
    vertices.emplace_back(l3);
    vertices.emplace_back(l3);
    vertices.emplace_back(l1);

    m_no_depth_lm.copy_to_permanent_buffer(vertices);
}

void MiniMap::add_temporary(size_t point_size, glm::vec3 const &pos,
                            glm::vec4 const &color) noexcept {
    float offset = 0.8f;

    /*
     * using same offset as edges, point might appear over the edge line,
     * if position is close to edge
     *
     * to fix this, reduce offset, or reduce offset based on point size, dont
     * know how to do it.
     *
     * minmap will be only used for notify position within map, so we will not
     * use any position, which will draw over edges
     *
     * */

    /*
     * SPACE 1
     *        (0.0, 0.0)     (1.0, 0.0)
     *        (0.0, 1.0)     (1.0, 1.0)
     *
     * SPACE 2 (opengl space)
     *
     *        (-offset,  offset)   (offset,  offset)
     *        (-offset, -offset)   (offset, -offset)
     * */

    /*convert pos to space 1*/

    glm::vec3 point_pos;

    point_pos.x = smoothstep(min_x_, max_x_, pos.x);
    point_pos.y = pos.y;
    point_pos.z = smoothstep(min_z_, max_z_, pos.z);

    /*convert to space 2*/

    point_pos.x = point_pos.x * (offset * 2.0f) - offset;
    point_pos.y = point_pos.z * (offset * -2.0f) + offset;
    point_pos.z = 0.0f;

    PrimitiveManager::Vertex point{true, static_cast<uint32_t>(point_size),
                                   point_pos, color};
    m_no_depth_pm.copy_to_temparary_buffer(&point, 1);
}

void MiniMap::tick(Context const &context) noexcept {
    m_no_depth_pm.tick(context);
    m_no_depth_lm.tick(context);
}
