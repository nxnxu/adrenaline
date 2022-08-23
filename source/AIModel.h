#ifndef ADRENALINE_AIMODEL_H
#define ADRENALINE_AIMODEL_H

#include <glm/glm.hpp>
#include <vector>

#include "context.h"
#include "importor.h"
#include "model.h"
#include "shader.h"
#include "texture.h"

class AIModel : public Model {
    /*AIModel consists of a head and a body*/
   public:
    AIModel(Importor::Model&& model, std::string const& mpart_head_name);
    AIModel(AIModel&&) = default;
    void render(Context const&, Shader&, glm::vec3 translation,
                glm::mat4 const& head_trans, glm::mat4 const& p_trans) noexcept;

   private:
    size_t head_id_{};
    std::vector<Texture> mpart_textures_;
    std::vector<glm::mat4> mpart_transformations_{10, glm::mat4{1.0f}};
};

#endif  // ADRENALINE_AIMODEL_H
