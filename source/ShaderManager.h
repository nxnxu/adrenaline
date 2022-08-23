#ifndef ADRENALINE_SHADERMANAGER_H
#define ADRENALINE_SHADERMANAGER_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "helper.h"
#include "shader.h"

class ShaderManager {
    using mat4 = glm::mat4;

   public:
    ShaderManager(std::string const& base_path);
    size_t add(std::string const& name);
    void use(size_t id);
    void set(std::string uname, mat4 const& mat);
    void set(std::string uname, GLint value);
    Shader& shader() noexcept;

   private:
    size_t index_{};
    std::string base_path_;
    std::vector<Shader> shaders_;
};

#endif  // ADRENALINE_SHADERMANAGER_H
