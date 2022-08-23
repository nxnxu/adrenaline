#ifndef ADRENALINE_MAZE_MANAGER_H
#define ADRENALINE_MAZE_MANAGER_H

#include <glad/glad.h>

#include <bitset>
#include <cstdio>
#include <glm/glm.hpp>
#include <random>
#include <stack>
#include <vector>

#include "context.h"
#include "random_generator.h"
#include "shader.h"

/**
 *
 *    "=" = Node                **************
 *    "*" = Wall                **==**==**==**
 *    PN = 3, PM = 2            **==**==**==**
 *    MW = 2, MH = 2            **************
 *    WX = 2, WZ = 1            **==**==**==**
 *                              **==**==**==**
 *                              **************
 */

struct MazeConfig {
    using vec3 = glm::vec3;
    size_t PN;  // number of nodes in x direction
    size_t PM;  // number of nodes in z direction
    size_t NW;  // number of boxes per node in x direction       (width of node)
    size_t
        NH;  // number of boxes per node in z direction       (height of node)
    size_t WX;  // number of boxes used to divide between nodes in x direction
    size_t WZ;  // number of boxes used to divide between nodes in z direction
    vec3 m_box_dimensions;  // dimension of the box
    vec3 m_map_base_pos;    // base position of the maze. Top-Left
};

class Maze;

class MazeInternal {
    friend Maze;

   public:
    size_t PN;
    size_t PM;
    size_t NW;
    size_t NH;
    size_t WX;
    size_t WZ;
    size_t m_num_nodes = PN * PM;
    size_t MAP_W = PN * NW + (1 + PN) * WX;
    size_t MAP_H = PM * NH + (1 + PM) * WZ;
    explicit MazeInternal(size_t pn, size_t pm, size_t nw, size_t nh,
                          size_t wxc, size_t wzc);
    size_t get_random_node() noexcept;
    size_t get_random_node(size_t node) noexcept;
    size_t get_path_next_node(size_t node_a, size_t node_b) const noexcept;
    std::pair<size_t, size_t> get_node_map_xz(size_t node) const noexcept;
    std::pair<size_t, size_t> get_node_map_xz(size_t nx,
                                              size_t nz) const noexcept;

   private:
    enum class NDIR { NORTH, SOUTH, EAST, WEST, NONE };
    std::uniform_int_distribution<size_t> node_dist_{0, m_num_nodes - 1};
    std::uniform_int_distribution<size_t> dir_dist_{0, 3};
    /*Stores next immediate node to visit from path nodeA to nodeB*/
    std::vector<std::vector<size_t>> path_next_node_;
    std::vector<std::vector<bool>> map_;
    std::vector<std::vector<size_t>> graph_;
    NDIR next_univisited_neighbour_NSEW(size_t node,
                                        std::vector<bool> const& visited);
    size_t get_neighbour_id(size_t node, NDIR direction);
    void create_node_space(size_t node);
    void break_wall(size_t node_a, size_t node_b, NDIR nsew);
    void break_north_wall(size_t node);
    void break_south_wall(size_t node);
    void break_east_wall(size_t node);
    void break_west_wall(size_t node);
    std::pair<size_t, size_t> get_node_xz(size_t node) const noexcept;
    bool are_neighbour(size_t node_a, size_t node_b) const noexcept;
};

class Maze {
    using vec3 = glm::vec3;

   public:
    size_t PN;
    size_t PM;
    size_t NW;
    size_t NH;
    size_t WX;
    size_t WZ;
    float m_bw_x;
    float m_bw_y;
    float m_bw_z;
    vec3 m_base_pos;
    explicit Maze(MazeConfig const& map_config);
    Maze(Maze const&) = delete;
    Maze(Maze&&) = delete;
    ~Maze();
    void render(Context const& context, Shader& shader) const noexcept;
    void render_path(Context const& context, Shader& shader) const noexcept;
    size_t get_random_node() noexcept;
    size_t get_random_node(size_t node) noexcept;
    size_t get_path_next_node(size_t node_a, size_t node_b) noexcept;
    glm::vec3 node_pos_center(size_t node) const noexcept;
    glm::vec3 node_pos_center(size_t nx, size_t nz) const noexcept;
    size_t nearest_node(glm::vec3 const& pos) const noexcept;
    glm::vec3 node_pos(size_t node) const noexcept;
    glm::vec3 mapxz_pos(size_t mx, size_t mz) const noexcept;
    bool is_wall(size_t mx, size_t mz) const noexcept;
    bool is_wall_dummy(size_t mx, size_t mz) const noexcept;
    size_t map_width() const noexcept;
    size_t map_height() const noexcept;
    float extreme_x() const noexcept { return maze_.MAP_W * m_bw_x; };
    float extreme_z() const noexcept { return maze_.MAP_H * m_bw_z; };

   private:
    size_t num_instance_{};
    GLuint vao_{};
    GLuint vbo_{};
    GLuint ebo_{};
    GLuint path_vao_{};
    GLuint path_vbo_{};
    size_t num_path_vertex_{};
    void build_path_mesh(std::vector<glm::vec3>& lp);
    void setup_path_mesh(std::vector<glm::vec3>& lp);
    MazeInternal maze_;
};

#endif  // ADRENALINE_MAZE_MANAGER_H
