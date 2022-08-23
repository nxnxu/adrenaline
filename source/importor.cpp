#include "importor.h"

namespace Importor {
Importor::Importor(std::string const &basepath, std::string const &filename)
    : m_basepath{basepath} {
    std::string path = basepath + get_path_separator() + filename;
    std::ifstream fin{path};
    fin.exceptions(std::ifstream::badbit);

    if (fin.is_open() == false) {
        throw std::runtime_error{path + "  cannot be opened"};
    }

    std::string key;
    fin >> key;

    if (key == this->SCENE) {
        this->read_header(fin, m_scene.m_header);
        return;
    }

    throw std::runtime_error{"IMPORTOR: unknown keyword"};
}

void Importor::read_header(std::ifstream &fin, Header &header) {
    std::string key;
    while (fin >> key) {
        if (key == LM) {
            this->read_lm(fin, header.m_loc_actions);
        } else if (key == this->LRSM) {
            this->read_lrsm(fin, header.m_lrs_actions);
        } else if (key == this->ANIMATION) {
            this->read_animation(fin, header.m_animations);
        } else if (key == this->DIFFUSEMAP) {
            this->read_diffuse_map(fin, header.m_diffuse_maps);
        } else if (key == this->MODEL) {
            m_scene.m_models.emplace_back();
            auto &model = m_scene.m_models.back();
            fin >> model.m_name;
            read_header(fin, model.m_header);
            return;
        } else if (key == this->MPART) {
            m_scene.m_models.back().m_mparts.emplace_back();

            auto &model = m_scene.m_models.back();
            auto &mpart = model.m_mparts.back();

            fin >> mpart.m_name;
            mpart.m_vertex_start_index =
                static_cast<uint32_t>(model.m_vertices.size());
            fin >> mpart.m_vertex_len;

            this->read_header(fin, mpart.m_header);
            return;
        } else if (key == this->VERTEX) {
            uint32_t mpart_index = static_cast<uint32_t>(
                m_scene.m_models.back().m_mparts.size() - 1);
            glm::vec3 pos = this->read_vec3(fin);
            glm::vec2 uv = this->read_vec2(fin);
            glm::vec3 nor = this->read_vec3(fin);
            m_scene.m_models.back().m_vertices.emplace_back(mpart_index, pos,
                                                            uv, nor);
        } else {
            throw std::runtime_error{"IMPORTOR: unknown keyword"};
        }
    }
}

glm::vec3 Importor::read_vec3(std::ifstream &fin) {
    glm::vec3 vec;
    fin >> vec.x >> vec.y >> vec.z;
    return vec;
}

glm::vec2 Importor::read_vec2(std::ifstream &fin) {
    glm::vec2 vec;
    fin >> vec.x >> vec.y;
    return vec;
}

glm::fquat Importor::read_fquat(std::ifstream &fin) {
    glm::fquat qua{};
    fin >> qua.w >> qua.x >> qua.y >> qua.z;
    return qua;
}

void Importor::read_lm(std::ifstream &fin,
                       std::vector<LocAction> &loc_markers) {
    std::string name;
    fin >> name;
    glm::vec3 loc = this->read_vec3(fin);
    loc_markers.emplace_back(name, loc);
}

void Importor::read_lrsm(std::ifstream &fin,
                         std::vector<LRSAction> &lrs_markers) {
    std::string name;
    fin >> name;

    glm::vec3 loc = this->read_vec3(fin);
    glm::fquat rot = this->read_fquat(fin);
    glm::vec3 scale = this->read_vec3(fin);
    lrs_markers.emplace_back(name, KeyFrame{loc, rot, scale});
}

void Importor::read_animation(std::ifstream &fin,
                              std::vector<Animation> &animations) {
    std::string name;
    fin >> name;
    uint32_t fps;
    fin >> fps;
    uint32_t frame_count;
    fin >> frame_count;

    Animation::frames_t frames{};

    for (uint32_t i = 0; i < frame_count; ++i) {
        std::string tmp;
        fin >> tmp;

        uint32_t frame_number;
        fin >> frame_number;

        glm::vec3 loc = this->read_vec3(fin);
        glm::fquat rot = this->read_fquat(fin);
        glm::vec3 scale = this->read_vec3(fin);

        frames.emplace_back(frame_number, KeyFrame{loc, rot, scale});
    }

    animations.emplace_back(name, fps, std::move(frames));
}

void Importor::read_diffuse_map(std::ifstream &fin,
                                std::vector<DiffuseMap> &diffuse_maps) {
    std::string name;
    fin >> name;

    std::string relative_path;
    fin >> relative_path;

    std::string path = m_basepath + get_path_separator() + relative_path;
    diffuse_maps.emplace_back(name, path);
}
}  // namespace Importor
