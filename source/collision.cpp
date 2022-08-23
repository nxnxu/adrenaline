#include "collision.h"

#include "helper.h"

void maze_collision_resolution_aabbxzmtv(Maze const& maze, MazeQuad const& quad,
                                         AABBXZMTV& aabbxzmtv,
                                         float max_allowed_depth,
                                         float max_threshold) noexcept

{
    /*calculate AABBXZ for current quad*/
    AABBXZ quad_aabb;
    quad_aabb.m_min_x = quad.m_pos.x;
    quad_aabb.m_min_z = quad.m_pos.z;
    quad_aabb.m_max_x = quad.m_pos.x + quad.m_w * maze.m_bw_x;
    quad_aabb.m_max_z = quad.m_pos.z + quad.m_h * maze.m_bw_z;

    /*Get final aabb by translating it in to its, respective axes*/
    AABBXZ query_aabb = aabbxzmtv.m_aabb;
    vec3 cmb_x = vec3{aabbxzmtv.m_xPositiveAxis ? 1.0f : -1.0f, 0.0f, 0.0f} *
                 aabbxzmtv.m_x_dt;
    vec3 cmb_z = vec3{0.0f, 0.0f, aabbxzmtv.m_zPositiveAxis ? 1.0f : -1.0f} *
                 aabbxzmtv.m_z_dt;
    vec3 cmb = cmb_x + cmb_z;
    query_aabb.translate(cmb);

    /*check if two aabbxz are colliding*/
    if (!AABBXZAABBXZ(quad_aabb, query_aabb, max_allowed_depth)) {
        return;
    }

    if (quad.m_w == 1) {
        /*reached lowest child*/
        if (!maze.is_wall_dummy(quad.m_mx, quad.m_mz)) {
            /* No wall, empty space*/
            return;
        }

        float x, z;
        AABBXZAABBXZ(quad_aabb, query_aabb, max_allowed_depth,
                     aabbxzmtv.m_xMove, aabbxzmtv.m_zMove,
                     aabbxzmtv.m_xPositiveAxis, aabbxzmtv.m_zPositiveAxis, &x,
                     &z);

        /*Make sure that resolution values are not greator than travelled
         * values*/
        bool afford_x = (x <= aabbxzmtv.m_x_dt + max_threshold);
        bool afford_z = (z <= aabbxzmtv.m_z_dt + max_threshold);

        // player aabb has moved in both x and z direction
        // Just moving one of the values will prevent collision.
        // Just move back which is smaller. This causes sliding effect when
        // walking towards wall.
        if (afford_x && afford_z) {
            if (x < z) {
                aabbxzmtv.m_x_dt = std::max(0.0f, aabbxzmtv.m_x_dt - x);
            } else {
                aabbxzmtv.m_z_dt = std::max(0.0f, aabbxzmtv.m_z_dt - z);
            }
        } else if (afford_x) {
            aabbxzmtv.m_x_dt = std::max(0.0f, aabbxzmtv.m_x_dt - x);
        } else if (afford_z) {
            aabbxzmtv.m_z_dt = std::max(0.0f, aabbxzmtv.m_z_dt - z);
        } else {
            /*Both resolution, values are greator than travelled values*/
            aabbxzmtv.m_x_dt = 0.0f;
            aabbxzmtv.m_z_dt = 0.0f;
        }
        return;
    }

    // child quads

    MazeQuad cQuad{quad.m_pos, quad.m_mx, quad.m_mz, quad.m_w >> 1,
                   quad.m_h >> 1};
    maze_collision_resolution_aabbxzmtv(maze, cQuad, aabbxzmtv,
                                        max_allowed_depth, max_threshold);

    cQuad.m_pos.x = quad.m_pos.x + (quad.m_w >> 1) * maze.m_bw_x;
    cQuad.m_pos.z = quad.m_pos.z;
    cQuad.m_mx = quad.m_mx + (quad.m_w >> 1);
    cQuad.m_mz = quad.m_mz;
    cQuad.m_w = quad.m_w >> 1;
    cQuad.m_h = quad.m_h >> 1;
    maze_collision_resolution_aabbxzmtv(maze, cQuad, aabbxzmtv,
                                        max_allowed_depth, max_threshold);

    cQuad.m_pos.x = quad.m_pos.x;
    cQuad.m_pos.z = quad.m_pos.z + (quad.m_h >> 1) * maze.m_bw_z;
    cQuad.m_mx = quad.m_mx;
    cQuad.m_mz = quad.m_mz + (quad.m_h >> 1);
    cQuad.m_w = quad.m_w >> 1;
    cQuad.m_h = quad.m_h >> 1;
    maze_collision_resolution_aabbxzmtv(maze, cQuad, aabbxzmtv,
                                        max_allowed_depth, max_threshold);

    cQuad.m_pos.x = quad.m_pos.x + (quad.m_w >> 1) * maze.m_bw_x;
    cQuad.m_pos.z = quad.m_pos.z + (quad.m_h >> 1) * maze.m_bw_z;
    cQuad.m_mx = quad.m_mx + (quad.m_w >> 1);
    cQuad.m_mz = quad.m_mz + (quad.m_h >> 1);
    cQuad.m_w = quad.m_w >> 1;
    cQuad.m_h = quad.m_h >> 1;
    maze_collision_resolution_aabbxzmtv(maze, cQuad, aabbxzmtv,
                                        max_allowed_depth, max_threshold);
}

void maze_collision_resolution_aabbxzmtv(Maze const& maze, AABBXZMTV& aabbxzmtv,
                                         size_t max_iterations,
                                         float max_allowed_depth,
                                         float max_threshold) noexcept {
    MazeQuad quad{};

    for (size_t i = 0; i < max_iterations; ++i) {
        quad.m_pos = maze.mapxz_pos(0, 0);
        quad.m_mx = 0;
        quad.m_mz = 0;
        quad.m_w = round_next_pow2(maze.map_width());
        quad.m_h = round_next_pow2(maze.map_height());
        maze_collision_resolution_aabbxzmtv(maze, quad, aabbxzmtv,
                                            max_allowed_depth, max_threshold);
    }
}

bool AABBXZAABBXZ(AABBXZ const& a, AABBXZ const& b,
                  float max_allowed_depth) noexcept {
    if (a.m_max_x <= b.m_min_x + max_allowed_depth ||
        b.m_max_x <= a.m_min_x + max_allowed_depth) {
        return false;
    }

    if (a.m_max_z <= b.m_min_z + max_allowed_depth ||
        b.m_max_z <= a.m_min_z + max_allowed_depth) {
        return false;
    }

    return true;
}

bool AABBXZAABBXZ(AABBXZ const& a, AABBXZ const& b, float max_allowed_depth,
                  bool b_xMove, bool b_zMove, bool b_xPositiveAxis,
                  bool b_zPositiveAxis, float* x, float* z) noexcept {
    if (a.m_max_x <= b.m_min_x + max_allowed_depth ||
        b.m_max_x <= a.m_min_x + max_allowed_depth) {
        return false;
    }

    if (a.m_max_z <= b.m_min_z + max_allowed_depth ||
        b.m_max_z <= a.m_min_z + max_allowed_depth) {
        return false;
    }

    if (b_xMove && x != nullptr) {
        if (b_xPositiveAxis) {
            *x = std::fabs(a.m_min_x - b.m_max_x);
        } else {
            *x = std::fabs(a.m_max_x - b.m_min_x);
        }
    } else if (x != nullptr) {
        *x = std::numeric_limits<float>::max();
    }

    if (b_zMove && z != nullptr) {
        if (b_zPositiveAxis) {
            *z = std::fabs(a.m_min_z - b.m_max_z);
        } else {
            *z = std::fabs(a.m_max_z - b.m_min_z);
        }
    } else if (z != nullptr) {
        *z = std::numeric_limits<float>::max();
    }

    return true;
}

void maze_line_collision_test(Maze const& maze, MazeQuad const& quad,
                              MazeLineCollisionResult& result) noexcept {
    if (quad.m_w == 1 && !maze.is_wall_dummy(quad.m_mx, quad.m_mz)) {
        return;
    }

    vec3 min_coord = quad.m_pos;
    vec3 max_coord = vec3(quad.m_pos.x + quad.m_w * maze.m_bw_x,
                          quad.m_pos.y + 1 * maze.m_bw_y,
                          quad.m_pos.z + quad.m_h * maze.m_bw_z);

    vec3 contact;

    bool collision =
        line_aabb_intersection(result.m_line_p1, result.m_line_p2,
                               AABB{min_coord, max_coord}, contact);

    if (!collision) {
        return;
    }

    /* Leaf quad */
    if (quad.m_w == 1) {
        float dist = glm::length(contact - result.m_line_p1);
        if (!result.m_is_intersecting || dist < result.m_dist) {
            /*we got lowest hit point from line point a*/
            result.m_is_intersecting = true;
            result.m_dist = dist;
            result.m_intersecting_point = contact;
        }
        return;
    }

    // child quads

    MazeQuad cQuad{quad.m_pos, quad.m_mx, quad.m_mz, quad.m_w >> 1,
                   quad.m_h >> 1};
    maze_line_collision_test(maze, cQuad, result);

    cQuad.m_pos.x = quad.m_pos.x + (quad.m_w >> 1) * maze.m_bw_x;
    cQuad.m_pos.z = quad.m_pos.z;
    cQuad.m_mx = quad.m_mx + (quad.m_w >> 1);
    cQuad.m_mz = quad.m_mz;
    cQuad.m_w = quad.m_w >> 1;
    cQuad.m_h = quad.m_h >> 1;
    maze_line_collision_test(maze, cQuad, result);

    cQuad.m_pos.x = quad.m_pos.x;
    cQuad.m_pos.z = quad.m_pos.z + (quad.m_h >> 1) * maze.m_bw_z;
    cQuad.m_mx = quad.m_mx;
    cQuad.m_mz = quad.m_mz + (quad.m_h >> 1);
    cQuad.m_w = quad.m_w >> 1;
    cQuad.m_h = quad.m_h >> 1;
    maze_line_collision_test(maze, cQuad, result);

    cQuad.m_pos.x = quad.m_pos.x + (quad.m_w >> 1) * maze.m_bw_x;
    cQuad.m_pos.z = quad.m_pos.z + (quad.m_h >> 1) * maze.m_bw_z;
    cQuad.m_mx = quad.m_mx + (quad.m_w >> 1);
    cQuad.m_mz = quad.m_mz + (quad.m_h >> 1);
    cQuad.m_w = quad.m_w >> 1;
    cQuad.m_h = quad.m_h >> 1;
    maze_line_collision_test(maze, cQuad, result);
}

MazeLineCollisionResult maze_line_collision_test(vec3 const& line_p1,
                                                 vec3 const& line_p2,
                                                 Maze& maze) noexcept {
    MazeLineCollisionResult result{line_p1, line_p2};
    MazeQuad quad{};
    quad.m_pos = maze.mapxz_pos(0, 0);
    quad.m_mx = 0;
    quad.m_mz = 0;
    quad.m_w = round_next_pow2(maze.map_width());
    quad.m_h = round_next_pow2(maze.map_height());
    maze_line_collision_test(maze, quad, result);
    return result;
}

bool line_aabb_intersection(vec3 const& line_p1, vec3 const& line_p2,
                            AABB const& aabb, vec3& contact) noexcept {
    /*
     * http://psgraphics.blogspot.com/2016/02/new-simple-ray-box-test-from-andrew.html
     * */

    float tmin = std::numeric_limits<float>::min();
    float tmax = std::numeric_limits<float>::max();

    vec3 segment = line_p2 - line_p1;
    vec3 segment_dir = glm::normalize(segment);

    for (int a = 0; a < 3; ++a) {
        // float invD = segment_dir[a]? (1.0f / segment_dir[a]): 0.0f;
        float invD = 1.0f / segment_dir[a];
        float t0 = (aabb.m_min[a] - line_p1[a]) * invD;
        float t1 = (aabb.m_max[a] - line_p1[a]) * invD;
        if (invD < 0.0f) {
            std::swap(t0, t1);
        }
        tmin = t0 > tmin ? t0 : tmin;
        tmax = t1 < tmax ? t1 : tmax;
        if (tmax <= tmin) {
            return false;
        }
    }

    if (tmin <= glm::length(segment)) {
        contact = line_p1 + segment_dir * tmin;
        return true;
    } else {
        return false;
    }
}
