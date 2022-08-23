#include "PrimitiveManager.h"

PrimitiveManager::PrimitiveManager(size_t max_primitives, GLenum type) noexcept
    : m_max_primitives{max_primitives}, m_primitive_type{type} {
    switch (m_primitive_type) {
        case GL_POINTS:
            primitive_size_ = 1;
            break;
        case GL_LINES:
            primitive_size_ = 2;
            break;
        case GL_TRIANGLES:
            primitive_size_ = 3;
            break;
        default:
            assert(0);
    }

    size_t total_vertices = m_max_primitives * primitive_size_;
    permanent_vertices_.resize(total_vertices);
    temporary_vertices_.resize(total_vertices);

    vao_.bind();
    vbo_.bind();

    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * total_vertices, nullptr,
                 GL_DYNAMIC_DRAW);
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT,
                           static_cast<GLsizei>(sizeof(Vertex)),
                           reinterpret_cast<void *>(offsetof(Vertex, m_ndc)));
    glVertexAttribIPointer(
        1, 1, GL_UNSIGNED_INT, static_cast<GLsizei>(sizeof(Vertex)),
        reinterpret_cast<void *>(offsetof(Vertex, m_point_size)));
    glVertexAttribPointer(
        2, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(sizeof(Vertex)),
        reinterpret_cast<void *>(offsetof(Vertex, m_position)));
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE,
                          static_cast<GLsizei>(sizeof(Vertex)),
                          reinterpret_cast<void *>(offsetof(Vertex, m_color)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
}

size_t PrimitiveManager::upload_primitives(Vertex *buffer,
                                           size_t size) noexcept {
    if (size == 0) return 0;

    size_t count =
        std::min(m_max_primitives - m_num_primitives, size / primitive_size_);
    size_t offset = m_num_primitives * primitive_size_ * sizeof(Vertex);
    size_t buffer_size = count * primitive_size_ * sizeof(Vertex);
    glBufferSubData(GL_ARRAY_BUFFER, static_cast<GLintptr>(offset),
                    static_cast<GLsizeiptr>(buffer_size), buffer);
    return count;
}

void PrimitiveManager::tick(Context const &context) noexcept {
    (void)context;
    vbo_.bind();

    if (permanent_buffer_size_) {
        permanent_upload_tasks.emplace_back(&permanent_vertices_[0],
                                            permanent_buffer_size_);
    }

    if (temporary_buffer_size_) {
        temporary_upload_tasks.emplace_back(&temporary_vertices_[0],
                                            temporary_buffer_size_);
    }

    for (auto &task : permanent_upload_tasks) {
        m_num_primitives += upload_primitives(task.first, task.second);
    }

    tmp_num_primitives_ = 0;
    for (auto &task : temporary_upload_tasks) {
        tmp_num_primitives_ += upload_primitives(task.first, task.second);
    }

    permanent_upload_tasks.clear();
    temporary_upload_tasks.clear();
    permanent_buffer_size_ = 0;
    temporary_buffer_size_ = 0;
}

void PrimitiveManager::render(Context const &context) noexcept {
    (void)context;
    size_t total_primitives = m_num_primitives + tmp_num_primitives_;

    if (!total_primitives) return;

    vao_.bind();
    glDrawArrays(m_primitive_type, 0,
                 static_cast<GLsizei>(total_primitives * primitive_size_));
}

void PrimitiveManager::copy_to_permanent_buffer(
    const std::vector<PrimitiveManager::Vertex> &v) noexcept {
    size_t max_vertices = m_max_primitives * primitive_size_;
    size_t occupied = m_num_primitives * primitive_size_ +
                      permanent_buffer_size_ + temporary_buffer_size_;
    size_t remain = max_vertices - occupied;
    remain = std::min(remain, v.size());
    if (remain) {
        memcpy(&permanent_vertices_[permanent_buffer_size_], &v[0],
               remain * sizeof(Vertex));
        permanent_buffer_size_ += remain;
    }
}

void PrimitiveManager::copy_to_temparary_buffer(
    const std::vector<PrimitiveManager::Vertex> &v) noexcept {
    copy_to_temparary_buffer(&v[0], v.size());
}

void PrimitiveManager::copy_to_temparary_buffer(
    PrimitiveManager::Vertex const *buffer, size_t size) noexcept {
    size_t max_vertices = m_max_primitives * primitive_size_;
    size_t occupied = m_num_primitives * primitive_size_ +
                      permanent_buffer_size_ + temporary_buffer_size_;
    size_t remain = max_vertices - occupied;
    remain = std::min(remain, size);
    if (remain) {
        memcpy(&temporary_vertices_[temporary_buffer_size_], buffer,
               remain * sizeof(Vertex));
        temporary_buffer_size_ += remain;
    }
}

void PrimitiveManager::register_copy_to_permanent_buffer(
    PrimitiveManager::Vertex *buffer, size_t size) {
    permanent_upload_tasks.emplace_back(buffer, size);
}

void PrimitiveManager::register_copy_to_temporary_buffer(
    PrimitiveManager::Vertex *buffer, size_t size) {
    temporary_upload_tasks.emplace_back(buffer, size);
}