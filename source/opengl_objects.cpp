#include "opengl_objects.h"

VAO::VAO() noexcept { glGenVertexArrays(1, &vao_); }

VAO::VAO(VAO&& vao) noexcept : vao_{vao.vao_} { vao.vao_ = 0; }

VAO::~VAO() noexcept { glDeleteVertexArrays(1, &vao_); }

void VAO::bind() const noexcept { glBindVertexArray(vao_); }

VBO::VBO() noexcept { glGenBuffers(1, &vbo_); }

VBO::VBO(VBO&& vbo) noexcept : vbo_{vbo.vbo_} { vbo.vbo_ = 0; }

VBO::~VBO() noexcept { glDeleteBuffers(1, &vbo_); }

void VBO::bind() const noexcept { glBindBuffer(GL_ARRAY_BUFFER, vbo_); }