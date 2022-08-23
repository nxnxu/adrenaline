#include "ShaderManager.h"

using namespace glm;

ShaderManager::ShaderManager(std::string const& base_path)
    : base_path_{base_path} {}

size_t ShaderManager::add(std::string const& name) {
    std::string v_path = base_path_ + get_path_separator() + name + "_v.glsl";
    std::string f_path = base_path_ + get_path_separator() + name + "_f.glsl";

    shaders_.emplace_back(v_path, f_path);
    return shaders_.size() - 1;
}

void ShaderManager::use(size_t id) {
    if (id >= shaders_.size()) {
        assert(0);
    }

    index_ = id;
    shaders_[index_].use();
}

void ShaderManager::set(std::string uname, mat4 const& mat) {
    auto loc = shaders_[index_].get_uniform_location(uname.c_str());
    shaders_[index_].set_4matv(loc, 1, GL_FALSE, &mat[0][0]);
}

void ShaderManager::set(std::string uname, GLint value) {
    auto loc = shaders_[index_].get_uniform_location(uname.c_str());
    shaders_[index_].set_intv(1, loc, 1, &value);
}

Shader& ShaderManager::shader() noexcept { return shaders_[index_]; }