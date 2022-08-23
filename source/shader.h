#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <exception>
#include <fstream>
#include <glm/glm.hpp>
#include <sstream>
#include <string>

class Shader {
   public:
    Shader(std::string const& ver_filepath, std::string const& frag_filepath);
    Shader(Shader const&) = delete;
    Shader(Shader&&) noexcept;
    ~Shader();
    GLint get_uniform_location(char const* name) const noexcept;
    void set_intv(GLint size, GLint location, GLsizei count,
                  GLint const* value) const noexcept;
    void set_floatv(GLint size, GLint location, GLsizei count,
                    GLfloat const* value) const noexcept;
    void set_4matv(GLint location, GLsizei count, GLboolean transpose,
                   GLfloat const* value) const noexcept;
    void use() const noexcept;

   private:
    GLuint m_program_id{};
    template <typename T>
    using type = void (*)(GLint, GLsizei, T const*);
    type<GLfloat> uniform_floatv_addr_[5] = {
        nullptr, glUniform1fv, glUniform2fv, glUniform3fv, glUniform4fv};
    type<GLint> uniform_intv_addr_[5] = {nullptr, glUniform1iv, glUniform2iv,
                                         glUniform3iv, glUniform4iv};
};

#endif  // SHADER_H
