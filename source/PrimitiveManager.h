#ifndef ADRENALINE_PRIMITIVEMANAGER_H
#define ADRENALINE_PRIMITIVEMANAGER_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <utility>
#include <vector>

#include "context.h"
#include "opengl_objects.h"
#include "shader.h"

class PrimitiveManager {
    using vec3 = glm::vec3;
    using vec4 = glm::vec4;

   public:
    struct Vertex {
        uint32_t m_ndc{};
        uint32_t m_point_size{};
        vec3 m_position{};
        vec4 m_color{};
        Vertex() = default;
        Vertex(uint32_t ndc, uint32_t point_size, vec3 const &pos,
               vec4 const &col)
            : m_ndc{ndc},
              m_point_size{point_size},
              m_position{pos},
              m_color{col} {}
    };

    size_t m_max_primitives{};
    size_t m_num_primitives{};
    GLenum m_primitive_type{};
    explicit PrimitiveManager(size_t max_primitives, GLenum type) noexcept;
    void copy_to_permanent_buffer(std::vector<Vertex> const &v) noexcept;
    void copy_to_temparary_buffer(std::vector<Vertex> const &v) noexcept;
    void copy_to_temparary_buffer(Vertex const *buffer, size_t size) noexcept;
    void register_copy_to_permanent_buffer(Vertex *buffer, size_t size);
    void register_copy_to_temporary_buffer(Vertex *buffer, size_t size);
    void tick(Context const &context) noexcept;
    void render(Context const &context) noexcept;

   private:
    VAO vao_{};
    VBO vbo_{};
    size_t tmp_num_primitives_{};
    size_t primitive_size_{};
    size_t permanent_buffer_size_{};
    size_t temporary_buffer_size_{};
    std::vector<Vertex> permanent_vertices_;
    std::vector<Vertex> temporary_vertices_;
    std::vector<std::pair<Vertex *, size_t>> permanent_upload_tasks;
    std::vector<std::pair<Vertex *, size_t>> temporary_upload_tasks;
    size_t upload_primitives(Vertex *buffer, size_t size) noexcept;
};

#endif  // ADRENALINE_PRIMITIVEMANAGER_H
