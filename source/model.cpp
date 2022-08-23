#include "model.h"

Model::Model(Importor::Model const& model)
    : m_name{model.m_name}, m_header{model.m_header}, m_mparts{model.m_mparts} {
    this->init(model.m_vertices);
}

Model::Model(Importor::Model&& model)
    : m_name{std::move(model.m_name)},
      m_header{std::move(model.m_header)},
      m_mparts{std::move(model.m_mparts)} {
    this->init(model.m_vertices);
}

void Model::init(std::vector<Importor::Vertex> const& vertices) {
    m_num_vertices = vertices.size();

    vao_.bind();
    vbo_.bind();

    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(sizeof(Importor::Vertex) * vertices.size()),
        &vertices[0], GL_STATIC_DRAW);
    glVertexAttribIPointer(
        0, 1, GL_UNSIGNED_INT, sizeof(Importor::Vertex),
        reinterpret_cast<void*>(offsetof(Importor::Vertex, m_mpart_id)));
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, sizeof(Importor::Vertex),
        reinterpret_cast<void*>(offsetof(Importor::Vertex, m_pos)));
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE, sizeof(Importor::Vertex),
        reinterpret_cast<void*>(offsetof(Importor::Vertex, m_uv)));
    glVertexAttribPointer(
        3, 3, GL_FLOAT, GL_FALSE, sizeof(Importor::Vertex),
        reinterpret_cast<void*>(offsetof(Importor::Vertex, m_nor)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
}

size_t Model::size() const noexcept { return m_mparts.size(); }
