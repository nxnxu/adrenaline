#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "header.h"
#include "importor.h"
#include "mpart.h"
#include "opengl_objects.h"

using KeyFrame = Importor::KeyFrame;

class Model {
   public:
    std::string m_name{};
    Header m_header{};
    size_t m_num_vertices{};
    std::vector<Mpart> m_mparts{};
    Model(Importor::Model const& model);
    Model(Importor::Model&& model);
    Model(const Model& model) = delete;
    Model(Model&&) = default;
    size_t size() const noexcept;

   protected:
    VAO vao_;
    VBO vbo_;

   private:
    void init(std::vector<Importor::Vertex> const& vertices);
};

#endif  // MODEL_H
