#ifndef ADRENALINE_GEOMETRICS_H
#define ADRENALINE_GEOMETRICS_H

#include <array>
#include <glm/glm.hpp>
#include <numeric>

#include "importor.h"

struct Plane {
    glm::vec3 m_point;
    glm::vec3 m_normal;

    Plane() = default;
    Plane(glm::vec3 const& a, glm::vec3 const& b, glm::vec3 const& c) noexcept
        : m_point{a}, m_normal{glm::normalize(glm::cross(b - a, c - a))} {}
};

struct AABBXZ {
    using vec3 = glm::vec3;

    float m_min_x{};
    float m_max_x{};
    float m_min_z{};
    float m_max_z{};
    AABBXZ() = default;
    AABBXZ(float min_x, float max_x, float min_z, float max_z)
    noexcept : m_min_x{min_x}, m_max_x{max_x}, m_min_z{min_z}, m_max_z{max_z} {}

    void translate(vec3 const& tv) noexcept {
        m_min_x += tv.x;
        m_max_x += tv.x;
        m_min_z += tv.z;
        m_max_z += tv.z;
    }
};

struct AABB {
    using vec3 = glm::vec3;

    vec3 m_min{};
    vec3 m_max{};
    AABB() = default;
    AABB(vec3 const& min, vec3 const& max) noexcept : m_min{min}, m_max{max} {}

    void translate(vec3 const& tv) noexcept {
        m_min += tv;
        m_max += tv;
    }
};

struct AABBPLANES {
    /*
     * Representing aabb sides with planes
     * */

    Plane m_planes[6];

    Plane& north() { return m_planes[0]; }
    Plane& south() { return m_planes[1]; }
    Plane& east() { return m_planes[2]; }
    Plane& west() { return m_planes[3]; }
    Plane& top() { return m_planes[4]; }
    Plane& bottom() { return m_planes[5]; }

    explicit AABBPLANES(AABB const& aabb) {
        glm::vec3 x = glm::vec3{aabb.m_min.x, aabb.m_max.y, aabb.m_max.z};
        glm::vec3 y = glm::vec3{aabb.m_max.x, aabb.m_min.y, aabb.m_max.z};
        glm::vec3 z = glm::vec3{aabb.m_max.x, aabb.m_max.y, aabb.m_min.z};

        glm::vec3 xy = glm::vec3{aabb.m_min.x, aabb.m_min.y, aabb.m_max.z};
        glm::vec3 xz = glm::vec3{aabb.m_min.x, aabb.m_max.y, aabb.m_min.z};
        glm::vec3 yz = glm::vec3{aabb.m_max.x, aabb.m_min.y, aabb.m_min.z};

        glm::vec3 xyz = glm::vec3{aabb.m_min.x, aabb.m_min.y, aabb.m_min.z};

        north() = {xyz, yz, z};
        south() = {xy, aabb.m_max, y};
        east() = {yz, y, aabb.m_max};
        west() = {xyz, x, xy};
        top() = {xz, aabb.m_max, x};
        bottom() = {xyz, xy, y};
    }
};

AABBXZ calculate_aabbxz(Importor::Model const& model) noexcept;
AABB calculate_aabb(Importor::Model const& model) noexcept;

#endif  // ADRENALINE_GEOMETRICS_H
