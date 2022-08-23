#include "geometrics.h"

using glm::vec3;

AABBXZ calculate_aabbxz(Importor::Model const& model) noexcept {
    float min_x = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::min();
    float min_z = std::numeric_limits<float>::max();
    float max_z = std::numeric_limits<float>::min();

    for (auto const& vertex : model.m_vertices) {
        min_x = std::min(min_x, vertex.m_pos.x);
        max_x = std::max(max_x, vertex.m_pos.x);
        min_z = std::min(min_z, vertex.m_pos.z);
        max_z = std::max(max_z, vertex.m_pos.z);
    }

    return AABBXZ{min_x, max_x, min_z, max_z};
}

AABB calculate_aabb(Importor::Model const& model) noexcept {
    vec3 min = vec3{std::numeric_limits<float>::max()};
    vec3 max = vec3{std::numeric_limits<float>::min()};

    for (auto const& vertex : model.m_vertices) {
        min.x = std::min(min.x, vertex.m_pos.x);
        min.y = std::min(min.y, vertex.m_pos.y);
        min.z = std::min(min.z, vertex.m_pos.z);

        max.x = std::max(max.x, vertex.m_pos.x);
        max.y = std::max(max.y, vertex.m_pos.y);
        max.z = std::max(max.z, vertex.m_pos.z);
    }

    return AABB{min, max};
}
