#ifndef ADRENALINE_COLLISION_H
#define ADRENALINE_COLLISION_H

#include "geometrics.h"
#include "maze.h"

using glm::vec3;

struct MazeQuad {
    vec3 m_pos{};   // world position of top-left.
    size_t m_mx{};  // maze internal map, x coordinate
    size_t m_mz{};  // maze internal map, z coordinate
    size_t m_w{};   // maze internal map, width, starting from m_mx  (N)
    size_t m_h{};   // maze internal map, height, starting from m_mz (M)

    MazeQuad() = default;
    MazeQuad(vec3 const& pos, size_t mx, size_t mz, size_t w, size_t h)
        : m_pos{pos}, m_mx{mx}, m_mz{mz}, m_w{w}, m_h{h} {}
};

struct AABBXZMTV {
    /*
     * Hold maximum distance AABBXZ can travel in X, Z direction without
     * collision.
     *
     * */
    AABBXZ m_aabb{};
    bool m_xMove{};
    bool m_zMove{};
    bool m_xPositiveAxis{};
    bool m_zPositiveAxis{};
    float m_x_dt{};
    float m_z_dt{};
    AABBXZMTV() = default;
};

struct MazeLineCollisionResult {
    vec3 m_line_p1{};
    vec3 m_line_p2{};
    bool m_is_intersecting{};
    vec3 m_intersecting_point{};
    float m_dist =
        std::numeric_limits<float>::max();  // holds distance from line_p1 to
                                            // intersecting point

    MazeLineCollisionResult(glm::vec3 const& a, glm::vec3 const& b) noexcept
        : m_line_p1{a}, m_line_p2{b} {}
};

/*return true if two AABBXZ, are colliding*/
bool AABBXZAABBXZ(AABBXZ const& a, AABBXZ const& b,
                  float max_allowed_depth) noexcept;

/*return true if b is colliding with a, and return intersection values in x, z
 * pointers*/
bool AABBXZAABBXZ(AABBXZ const& a, AABBXZ const& b,
                  float max_allowed_depth, /* Allowed to intersect */
                  bool b_xMove, bool b_zMove, bool b_xPositiveAxis,
                  bool b_zPositiveAxis, float* x, float* z) noexcept;

/*corrects the x_dt, z_dt values of AABBXZMTV, such that it doesn't collides
 * with any boxes inside maze*/
void maze_collision_resolution_aabbxzmtv(
    Maze const& maze, MazeQuad const& quad, AABBXZMTV& aabbxzmtv,
    float max_allowed_depth, /* Allowed to intersect */
    float max_threshold      /* Small epsilon value */
    ) noexcept;

/*corrects the x_dt, z_dt values of AABBXZMTV, such that it doesn't collides
 * with any boxes inside maze */
void maze_collision_resolution_aabbxzmtv(Maze const& maze, AABBXZMTV& aabbxzmtv,
                                         size_t max_iterations,
                                         float max_allowed_depth,
                                         float max_threshold) noexcept;

/*find the closet intersecting point of line with boxes inside maze, if
 * colliding*/
void maze_line_collision_test(Maze const& maze, MazeQuad const& quad,
                              MazeLineCollisionResult& result) noexcept;

/*returns a MazeLineCollisionResult, contaning the closet intersecting point of
 * a line, with any boxes inside maze */
MazeLineCollisionResult maze_line_collision_test(vec3 const& line_p1,
                                                 vec3 const& line_p2,
                                                 Maze& maze) noexcept;

bool line_aabb_intersection(vec3 const& line_p1, vec3 const& line_p2,
                            AABB const& aabb, vec3& contact) noexcept;

#endif  // ADRENALINE_COLLISION_H
