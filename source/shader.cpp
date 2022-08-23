#include "shader.h"

namespace {
using param_func = void (*)(GLuint, GLenum, GLint *);
using info_func = void (*)(GLuint, GLsizei, GLsizei *, GLchar *);

void check_errors(GLuint sp, GLenum param, param_func parameter,
                  info_func infolog) {
    GLint res;
    parameter(sp, param, &res);
    if (res == GL_FALSE) {
        constexpr size_t buffer_len = 1025;
        constexpr GLsizei reserved = 8;
        char buffer[buffer_len] = "SHADER: ";
        GLsizei info_len;
        infolog(sp, buffer_len - reserved - 1, &info_len, buffer + reserved);
        buffer[reserved + info_len + 1] = '\0';
        throw std::runtime_error{std::string{buffer}};
    }
}
}  // namespace

Shader::Shader(std::string const &ver_filepath,
               std::string const &frag_filepath) {
    std::ifstream v_in{ver_filepath};
    std::ifstream f_in{frag_filepath};

    v_in.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    f_in.exceptions(std::ifstream::badbit | std::ifstream::failbit);

    if (v_in.is_open() == false) {
        throw std::runtime_error{ver_filepath + " could not open"};
    }
    if (f_in.is_open() == false) {
        throw std::runtime_error{frag_filepath + " could not open"};
    }

    std::stringstream v_stream, f_stream;
    v_stream << v_in.rdbuf();
    f_stream << f_in.rdbuf();

    std::string v_string = v_stream.str();
    std::string f_string = f_stream.str();

    const char *v_source = v_string.c_str();
    const char *f_source = f_string.c_str();

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertex, 1, &v_source, nullptr);
    glShaderSource(fragment, 1, &f_source, nullptr);
    glCompileShader(vertex);
    glCompileShader(fragment);

    check_errors(vertex, GL_COMPILE_STATUS, glGetShaderiv, glGetShaderInfoLog);
    check_errors(fragment, GL_COMPILE_STATUS, glGetShaderiv,
                 glGetShaderInfoLog);

    m_program_id = glCreateProgram();
    glAttachShader(m_program_id, vertex);
    glAttachShader(m_program_id, fragment);
    glLinkProgram(m_program_id);

    check_errors(m_program_id, GL_LINK_STATUS, glGetProgramiv,
                 glGetProgramInfoLog);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::Shader(Shader &&shader) noexcept : m_program_id{shader.m_program_id} {
    shader.m_program_id = 0;
}

Shader::~Shader() { glDeleteProgram(m_program_id); }

void Shader::use() const noexcept { glUseProgram(m_program_id); }

GLint Shader::get_uniform_location(char const *name) const noexcept {
    return glGetUniformLocation(m_program_id, name);
}

void Shader::set_intv(GLint size, GLint location, GLsizei count,
                      const GLint *value) const noexcept {
    uniform_intv_addr_[size](location, count, value);
}

void Shader::set_floatv(GLint size, GLint location, GLsizei count,
                        const GLfloat *value) const noexcept {
    uniform_floatv_addr_[size](location, count, value);
}

void Shader::set_4matv(GLint location, GLsizei count, GLboolean transpose,
                       const GLfloat *value) const noexcept {
    glUniformMatrix4fv(location, count, transpose, value);
}
