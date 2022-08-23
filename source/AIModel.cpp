#include "AIModel.h"

using glm::mat4;
using glm::vec3;

AIModel::AIModel(Importor::Model&& model, std::string const& mpart_head_name)
    : Model{std::move(model)} {
    for (auto& mpart : m_mparts) {
        auto& mpart_skin_diffuse_map = mpart.m_header.m_diffuse_maps[0];
        mpart_textures_.emplace_back(mpart_skin_diffuse_map.m_path);
    }

    for (size_t i = 0; i < m_mparts.size(); ++i) {
        if (m_mparts[i].m_name == mpart_head_name) {
            head_id_ = i;
            return;
        }
    }

    throw std::runtime_error{"could not find mpart named: " + mpart_head_name};
}

void AIModel::render(Context const&, Shader& shader, vec3 translation,
                     mat4 const& head_trans, mat4 const& p_trans) noexcept {
    for (size_t i = 0; i < mpart_transformations_.size(); ++i) {
        if (i == head_id_) {
            mpart_transformations_[i] =
                glm::translate(mat4{1.0f}, translation) * head_trans;
        } else {
            mpart_transformations_[i] = glm::translate(mat4{1.0f}, translation);
        }
    }

    vao_.bind();
    static GLint samplers[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    auto mpart_trans_loc = shader.get_uniform_location("mpart_trans");
    auto samplers_loc = shader.get_uniform_location("samplers");
    auto p_trans_loc = shader.get_uniform_location("pTrans");

    shader.set_intv(1, samplers_loc, 10, samplers);

    shader.set_4matv(mpart_trans_loc,
                     static_cast<GLsizei>(mpart_transformations_.size()),
                     GL_FALSE, &mpart_transformations_[0][0][0]);

    shader.set_4matv(p_trans_loc, 1, GL_FALSE, &p_trans[0][0]);

    for (size_t i = 0; i < mpart_textures_.size(); ++i) {
        mpart_textures_[i].bind_to_texture_unit(i);
    }

    glDrawArrays(GL_TRIANGLES, 0, m_num_vertices);
}