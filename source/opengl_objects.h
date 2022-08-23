#ifndef ADRENALINE_OPENGL_OBJECTS_H
#define ADRENALINE_OPENGL_OBJECTS_H

#include <glad/glad.h>

class VAO {
   public:
    explicit VAO() noexcept;
    VAO(VAO const&) = delete;
    VAO(VAO&& vao) noexcept;
    ~VAO() noexcept;
    void bind() const noexcept;

   private:
    GLuint vao_{};
};

class VBO {
   public:
    explicit VBO() noexcept;
    VBO(VBO const&) = delete;
    VBO(VBO&& vbo) noexcept;
    ~VBO() noexcept;
    void bind() const noexcept;

   private:
    GLuint vbo_{};
};

#endif  // ADRENALINE_OPENGL_OBJECTS_H
