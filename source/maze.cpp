#include "maze.h"

#include "helper.h"

MazeInternal::MazeInternal(size_t pn, size_t pm, size_t nw, size_t nh,
                           size_t wxc, size_t wzc)
    : PN{pn},
      PM{pm},
      NW{nw},
      NH{nh},
      WX{wxc},
      WZ{wzc},
      path_next_node_{m_num_nodes},
      map_{MAP_H},
      graph_{m_num_nodes} {
    for (size_t i = 0; i < m_num_nodes; ++i) {
        path_next_node_[i].resize(m_num_nodes);
    }

    for (size_t i = 0; i < MAP_H; ++i) {
        map_[i].resize(MAP_W, false);
    }

    std::vector<bool> visited(m_num_nodes, false);
    std::stack<size_t> st;

    using T = std::vector<size_t>;
    std::vector<T> dist(m_num_nodes, T(m_num_nodes, m_num_nodes + 1));
    std::vector<T> wchk(m_num_nodes, T(m_num_nodes, m_num_nodes));

    for (size_t i = 0; i < m_num_nodes; ++i) {
        dist[i][i] = 0;
        wchk[i][i] = i;
        graph_[i].push_back(i);
    }

    st.push(node_dist_(random_generator.m_gen));
    visited[st.top()] = true;
    while (!st.empty()) {
        auto node = st.top();
        auto next_dir = next_univisited_neighbour_NSEW(node, visited);
        if (next_dir == NDIR::NONE) {
            st.pop();
            continue;
        }
        auto next_node = get_neighbour_id(node, next_dir);
        visited[next_node] = true;
        break_wall(node, next_node, next_dir);
        st.push(next_node);
        dist[node][next_node] = dist[next_node][node] = 1;
        wchk[node][next_node] = next_node;
        wchk[next_node][node] = node;
        graph_[node].push_back(next_node);
        graph_[next_node].push_back(node);
    }

    for (size_t i = 0; i < m_num_nodes; ++i) {
        create_node_space(i);
    }

    for (size_t k = 0; k < m_num_nodes; ++k) {
        for (size_t i = 0; i < m_num_nodes; ++i) {
            for (size_t j = 0; j < m_num_nodes; ++j) {
                if (dist[i][j] > dist[i][k] + dist[k][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    wchk[i][j] = k;
                }
            }
        }
    }

    for (size_t i = 0; i < m_num_nodes; ++i) {
        for (size_t j = 0; j < m_num_nodes; ++j) {
            auto next = wchk[i][j];
            while (wchk[i][next] != next) {
                next = wchk[i][next];
            }
            path_next_node_[i][j] = next;
        }
    }
}

size_t MazeInternal::get_random_node() noexcept {
    return node_dist_(random_generator.m_gen);
}

size_t MazeInternal::get_random_node(size_t node) noexcept {
    auto rnd_node = node_dist_(random_generator.m_gen);
    if (rnd_node == node) {
        rnd_node = (rnd_node + 1) % (PN * PM);
    }
    return rnd_node;
}

size_t MazeInternal::get_path_next_node(size_t node_a,
                                        size_t node_b) const noexcept {
    return path_next_node_[node_a][node_b];
}

typename MazeInternal::NDIR MazeInternal::next_univisited_neighbour_NSEW(
    size_t node, std::vector<bool> const& visited) {
    NDIR legal_neighbour[4];
    size_t index = 0;
    if (node >= PN && visited[node - PN] == 0) {
        legal_neighbour[index++] = NDIR::NORTH;
    }
    if (node + PN < PN * PM && visited[node + PN] == 0) {
        legal_neighbour[index++] = NDIR::SOUTH;
    }
    if ((node + 1) % PN != 0 && visited[node + 1] == 0) {
        legal_neighbour[index++] = NDIR::EAST;
    }
    if (node % PN != 0 && visited[node - 1] == 0) {
        legal_neighbour[index++] = NDIR::WEST;
    }
    if (index == 0) return NDIR::NONE;
    return legal_neighbour[dir_dist_(random_generator.m_gen) % index];
}

size_t MazeInternal::get_neighbour_id(size_t node,
                                      MazeInternal::NDIR direction) {
    if (direction == NDIR::NORTH) return node - PN;
    if (direction == NDIR::SOUTH) return node + PN;
    if (direction == NDIR::EAST) return node + 1;
    if (direction == NDIR::WEST) return node - 1;
    throw std::runtime_error{"get_neighbour_id: None Direction"};
}

void MazeInternal::create_node_space(size_t node) {
    auto map_xz = get_node_map_xz(node);
    auto nx = map_xz.first;
    auto nz = map_xz.second;

    for (size_t x = 0; x < NW; ++x) {
        for (size_t z = 0; z < NH; ++z) {
            map_[nz + z][nx + x] = true;
        }
    }
}

void MazeInternal::break_wall(size_t node_a, size_t node_b,
                              MazeInternal::NDIR nsew) {
    if (nsew == NDIR::EAST) return break_east_wall(node_a);
    if (nsew == NDIR::WEST) return break_east_wall(node_b);
    if (nsew == NDIR::SOUTH) return break_south_wall(node_a);
    if (nsew == NDIR::NORTH) return break_south_wall(node_b);

    throw std::runtime_error{"breal wall: None Direction"};
}

void MazeInternal::break_east_wall(size_t node) {
    auto map_xz = get_node_map_xz(node);
    auto wx = map_xz.first + NW;
    auto wz = map_xz.second;

    for (size_t x = 0; x < WX; ++x) {
        for (size_t z = 0; z < NH; ++z) {
            map_[wz + z][wx + x] = true;
        }
    }
}

void MazeInternal::break_west_wall(size_t node) {
    auto map_xz = get_node_map_xz(node);
    auto wx = map_xz.first;
    auto wz = map_xz.second;

    for (size_t x = 0; x < WX; ++x) {
        for (size_t z = 0; z < NH; ++z) {
            map_[wz + z][wx - x - 1] = true;
        }
    }
}

void MazeInternal::break_north_wall(size_t node) {
    auto map_xz = get_node_map_xz(node);
    auto wx = map_xz.first;
    auto wz = map_xz.second;

    for (size_t x = 0; x < NW; ++x) {
        for (size_t z = 0; z < WZ; ++z) {
            map_[wz - z - 1][wx + x] = true;
        }
    }
}

void MazeInternal::break_south_wall(size_t node) {
    auto map_xz = get_node_map_xz(node);
    auto wx = map_xz.first;
    auto wz = map_xz.second + NH;

    for (size_t x = 0; x < NW; ++x) {
        for (size_t z = 0; z < WZ; ++z) {
            map_[wz + z][wx + x] = true;
        }
    }
}

std::pair<size_t, size_t> MazeInternal::get_node_xz(
    size_t node) const noexcept {
    /*return (x, z) coord of node*/
    return std::make_pair(node % PN, node / PN);
}

bool MazeInternal::are_neighbour(size_t node_a, size_t node_b) const noexcept {
    if (node_a >= PN && node_a - PN == node_b) return true;
    if (node_a + PN < PN * PM && node_a + PN == node_b) return true;
    if ((node_a + 1) % PN != 0 && node_a + 1 == node_b) return true;
    if (node_a % PN != 0 && node_a - 1 == node_b) return true;
    return false;
}

std::pair<size_t, size_t> MazeInternal::get_node_map_xz(
    size_t node) const noexcept {
    auto xz = get_node_xz(node);
    size_t wx = NW * xz.first + WX + WX * xz.first;
    size_t wz = NH * xz.second + WZ + WZ * xz.second;
    return std::make_pair(wx, wz);
}

std::pair<size_t, size_t> MazeInternal::get_node_map_xz(
    size_t nx, size_t nz) const noexcept {
    size_t wx = NW * nx + WX + WX * nx;
    size_t wz = NH * nz + WZ + WZ * nz;
    return std::make_pair(wx, wz);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Maze::Maze(MazeConfig const& map_config)
    : PN{map_config.PN},
      PM{map_config.PM},
      NW{map_config.NW},
      NH{map_config.NH},
      WX{map_config.WX},
      WZ{map_config.WZ},
      m_bw_x{map_config.m_box_dimensions.x},
      m_bw_y{map_config.m_box_dimensions.y},
      m_bw_z{map_config.m_box_dimensions.z},
      m_base_pos{map_config.m_map_base_pos},
      maze_{MazeInternal(PN, PM, NW, NH, WX, WZ)} {
    std::vector<glm::vec3> lp{};
    build_path_mesh(lp);
    setup_path_mesh(lp);
    num_path_vertex_ = lp.size();

    static float boxv[] = {
        0.0f, 0.0f,   0.0f,   m_bw_x, 0.0f,   0.0f, m_bw_x, m_bw_y,
        0.0f, 0.0f,   m_bw_y, 0.0f,   0.0f,   0.0f, m_bw_z, m_bw_x,
        0.0f, m_bw_z, m_bw_x, m_bw_y, m_bw_z, 0.0f, m_bw_y, m_bw_z,
    };

    struct Instance {
        glm::vec3 m_pos{};
        Instance(float x, float y, float z) : m_pos{x, y, z} {}
    };

    std::vector<Instance> instances;

    for (size_t z = 0; z < maze_.MAP_H; ++z) {
        for (size_t x = 0; x < maze_.MAP_W; ++x) {
            if (maze_.map_[z][x]) continue;
            float tx = m_base_pos.x + m_bw_x * x;
            float ty = m_base_pos.y;
            float tz = m_base_pos.z + m_bw_z * z;
            instances.emplace_back(tx, ty, tz);
        }
    }

    num_instance_ = instances.size();

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(boxv) + sizeof(Instance) * instances.size(), nullptr,
                 GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(boxv), boxv);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(boxv),
                    sizeof(Instance) * instances.size(), &instances[0].m_pos.x);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<void*>(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<void*>(sizeof(boxv)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    /*Indices in CCW order*/
    static uint32_t draw_indices[] = {
        /* Traingle indices*/
        4, 5, 6, 6, 7, 4,
        1, 0, 3, 3, 2, 1,
        5, 1, 2, 2, 6, 5,
        0, 4, 7, 7, 3, 0,
        7, 6, 2, 2, 3, 7,
        0, 1, 5, 5, 4, 0,
        /* Line indices */
        0, 1, 1, 2, 2, 3,
        3, 0, 4, 5, 5, 6,
        6, 7, 7, 4, 5, 1,
        1, 2, 2, 6, 6, 5,
        0, 4, 4, 7, 7, 3,
        3, 0, 0, 1, 1, 5,
        5, 4, 4, 0, 7, 6,
        6, 2, 2, 3, 3, 7,
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(draw_indices), nullptr,
                 GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(draw_indices),
                    draw_indices);
}

void Maze::build_path_mesh(std::vector<glm::vec3>& lp) {
    for (size_t i = 0; i < PN * PM; ++i) {
        auto pos_a = node_pos_center(i);
        for (size_t c : maze_.graph_[i]) {
            lp.push_back(pos_a);
            lp.push_back(node_pos_center(c));
        }
    }
}

void Maze::setup_path_mesh(std::vector<glm::vec3>& lp) {
    glGenVertexArrays(1, &path_vao_);
    glGenBuffers(1, &path_vbo_);
    glBindVertexArray(path_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, path_vbo_);
    glBufferData(GL_ARRAY_BUFFER, 12 * lp.size(), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 12 * lp.size(), &lp[0].x);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<GLvoid*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttrib4f(1, 0.0f, 0.0f, 0.0f, 0.0f);
}

void Maze::render_path(Context const& context, Shader& shader) const noexcept {
    (void)context;
    static float red[] = {1.0f, 0.0f, 0.0f, 0.0f};
    auto col_loc = shader.get_uniform_location("color");
    shader.set_floatv(4, col_loc, 1, red);
    glBindVertexArray(path_vao_);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(num_path_vertex_));
}

Maze::~Maze() {
    glDeleteBuffers(1, &ebo_);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &path_vbo_);
    glDeleteVertexArrays(1, &path_vao_);
    glDeleteVertexArrays(1, &vao_);
}

void Maze::render(Context const& context, Shader& shader) const noexcept {
    (void)context;
    static float black_color[] = {0.0f, 0.0f, 0.0f, 1.0f};
    static float green_color[] = {0.0f, 1.0f, 0.0f, 1.0f};
    auto color_uid = shader.get_uniform_location("color");
    glBindVertexArray(vao_);
    shader.set_floatv(4, color_uid, 1, green_color);
    glDrawElementsInstanced(GL_LINES, 48, GL_UNSIGNED_INT,
                            reinterpret_cast<void*>(36 * sizeof(uint32_t)),
                            static_cast<GLsizei>(num_instance_));
    shader.set_floatv(4, color_uid, 1, black_color);
    glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT,
                            reinterpret_cast<void*>(0),
                            static_cast<GLsizei>(num_instance_));
}

glm::vec3 Maze::node_pos_center(size_t node) const noexcept {
    auto map_xz = maze_.get_node_map_xz(node);
    float x = m_base_pos.x + map_xz.first * m_bw_x + (NW / 2.0f) * m_bw_x;
    float y = m_base_pos.y + 0.0f;
    float z = m_base_pos.z + map_xz.second * m_bw_z + (NH / 2.0f) * m_bw_z;
    return glm::vec3{x, y, z};
}

glm::vec3 Maze::node_pos_center(size_t nx, size_t nz) const noexcept {
    auto map_xz = maze_.get_node_map_xz(nx, nz);
    float x = m_base_pos.x + map_xz.first * m_bw_x + (NW / 2.0f) * m_bw_x;
    float y = m_base_pos.y + 0.0f;
    float z = m_base_pos.z + map_xz.second * m_bw_z + (NH / 2.0f) * m_bw_z;
    return glm::vec3{x, y, z};
}

glm::vec3 Maze::node_pos(size_t node) const noexcept {
    auto map_xz = maze_.get_node_map_xz(node);
    float x = m_base_pos.x + map_xz.first * m_bw_x;
    float y = m_base_pos.y + 0.0f;
    float z = m_base_pos.z + map_xz.second * m_bw_z;
    return glm::vec3{x, y, z};
}

size_t Maze::get_random_node() noexcept { return maze_.get_random_node(); }

size_t Maze::get_random_node(size_t node) noexcept {
    return maze_.get_random_node(node);
}

glm::vec3 Maze::mapxz_pos(size_t mx, size_t mz) const noexcept {
    float x = m_base_pos.x + mx * m_bw_x;
    float y = m_base_pos.y + 0.0f;
    float z = m_base_pos.z + mz * m_bw_z;
    return glm::vec3{x, y, z};
}

size_t Maze::get_path_next_node(size_t node_a, size_t node_b) noexcept {
    return maze_.get_path_next_node(node_a, node_b);
}

bool Maze::is_wall(size_t mx, size_t mz) const noexcept {
    return !maze_.map_[mz][mx];
}

bool Maze::is_wall_dummy(size_t mx, size_t mz) const noexcept {
    if (mx >= maze_.MAP_W || mz >= maze_.MAP_H) return false;
    return !maze_.map_[mz][mx];
}

size_t Maze::map_width() const noexcept { return maze_.MAP_W; }

size_t Maze::map_height() const noexcept { return maze_.MAP_H; }

size_t Maze::nearest_node(glm::vec3 const& pos) const noexcept {
    /*
     * finds nearest node to given pos
     *
     * nodes with be divided in form of quads, nearest quad to pos is found,
     * and that quad is again sub divided in to another 4 quads and process goes
     * on.
     * */

    struct Quad {
        size_t m_nx;  // node x coordinate
        size_t m_nz;  // node z coordinate

        size_t m_w;  // number of nodes in x direction
        size_t m_h;  // number of nodes in z direction

        glm::vec3 center{};  // center point of quad

        Quad(Maze const& maze, size_t nx, size_t nz, size_t w,
             size_t h) noexcept
            : m_nx{nx}, m_nz{nz}, m_w{w}, m_h{h} {
            /*top-most left doesn't exists, so skip calculating its center*/

            if (m_nx >= maze.PN || m_nz >= maze.PM) {
                return;
            }

            glm::vec3 quad_pos = maze.node_pos_center(nx, nz);
            glm::vec3 extreme_x = maze.node_pos_center(nx + w - 1, nz);
            glm::vec3 extreme_z = maze.node_pos_center(nx, nz + h - 1);

            float x_len = glm::length(extreme_x - quad_pos) / 2.0f;
            float z_len = glm::length(extreme_z - quad_pos) / 2.0f;

            center.x = quad_pos.x + x_len;
            center.y = quad_pos.y;
            center.z = quad_pos.z + z_len;
        }
    };

    Quad quad{*this, 0, 0, round_next_pow2(PN), round_next_pow2(PM)};

    while (quad.m_w != 1) {
        size_t hw = quad.m_w >> 1u;
        size_t hh = quad.m_h >> 1u;
        Quad quads[4] = {
            {*this, quad.m_nx, quad.m_nz, hw, hh},
            {*this, quad.m_nx + hw, quad.m_nz, hw, hh},
            {*this, quad.m_nx, quad.m_nz + hh, hw, hh},
            {*this, quad.m_nx + hw, quad.m_nz + hh, hw, hh},
        };

        float nearest_dist = std::numeric_limits<float>::max();

        for (auto const& q : quads) {
            float dist = (q.m_nx >= PN || q.m_nz >= PM)
                             ? std::numeric_limits<float>::max()
                             : glm::length(q.center - pos);
            if (dist < nearest_dist) {
                nearest_dist = dist;
                quad = q;
            }
        }
    }

    return quad.m_nx + PN * quad.m_nz;
}