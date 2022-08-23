#ifndef IMPORTOR_H
#define IMPORTOR_H

#include <exception>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <map>
#include <string>
#include <vector>

#include "helper.h"

namespace Importor {
struct DiffuseMap {
    std::string m_name{};
    std::string m_path{};

    DiffuseMap() = default;
    DiffuseMap(std::string const& name, std::string const& path)
        : m_name{name}, m_path{path} {}
};

struct KeyFrame {
    glm::vec3 m_loc{};
    glm::fquat m_rot{};
    glm::vec3 m_scale{};
    KeyFrame() = default;
    KeyFrame(glm::vec3 const& loc, glm::fquat const& rot,
             glm::vec3 const& scale)
        : m_loc{loc}, m_rot{rot}, m_scale{scale} {}
};

struct LocAction {
    std::string m_name{};
    glm::vec3 m_loc{};
    LocAction(std::string const& name, glm::vec3 const& loc)
        : m_name{name}, m_loc{loc} {}
};

struct LRSAction {
    std::string m_name{};
    KeyFrame m_key_frame{};
    LRSAction() = default;
    LRSAction(std::string const& name, KeyFrame const& key_frame)
        : m_name{name}, m_key_frame{key_frame} {}
};

struct Animation {
    using frames_t = std::vector<std::pair<uint32_t, KeyFrame>>;
    std::string m_name{};
    uint32_t m_fps{};
    frames_t m_frames{};
    Animation() = default;
    template <typename T = frames_t>
    Animation(std::string const& name, uint32_t fps, T&& frames)
        : m_name{name}, m_fps{fps}, m_frames{std::forward<T>(frames)} {}
};

struct Vertex {
    uint32_t m_mpart_id{};
    glm::vec3 m_pos{};
    glm::vec2 m_uv{};
    glm::vec3 m_nor{};
    Vertex() = default;
    Vertex(uint32_t mpart_id, glm::vec3 const& pos, glm::vec2 const& uv,
           glm::vec3 const& nor)
        : m_mpart_id{mpart_id}, m_pos{pos}, m_uv{uv}, m_nor{nor} {}
};

struct Header {
    std::vector<DiffuseMap> m_diffuse_maps{};
    std::vector<LocAction> m_loc_actions{};
    std::vector<LRSAction> m_lrs_actions{};
    std::vector<Animation> m_animations{};
    template <typename T>
    auto get_action(T& action, std::string const& name) ->
        typename T::value_type& {
        for (size_t i = 0; i < action.size(); ++i) {
            if (action[i].m_name == name) return action[i];
        }
        throw std::runtime_error{"Find Action: cannot find action " + name};
    }
    LRSAction& get_lrs_action(std::string const& name) {
        return get_action(m_lrs_actions, name);
    }
    LocAction& get_loc_action(std::string const& name) {
        return get_action(m_loc_actions, name);
    }
    Animation& get_animation_action(std::string const& name) {
        return get_action(m_animations, name);
    }
    Header() = default;
};

struct Mpart {
    std::string m_name{};
    Header m_header{};
    uint32_t m_vertex_start_index{};
    uint32_t m_vertex_len{};
    Mpart() = default;
};

struct Model {
    std::string m_name{};
    Header m_header{};
    std::vector<Vertex> m_vertices{};
    std::vector<Mpart> m_mparts{};
    Model() = default;

    Mpart& get_mpart(std::string const& name) & {
        for (auto& mpart : m_mparts) {
            if (mpart.m_name == name) {
                return mpart;
            }
        }

        throw std::runtime_error{"Model: error while finding mpart" + name};
    }

    Mpart&& get_mpart(std::string const& name) && {
        for (auto& mpart : m_mparts) {
            if (mpart.m_name == name) {
                return std::move(mpart);
            }
        }

        throw std::runtime_error{"Model: error while finding mpart" + name};
    }
};

struct Scene {
    Header m_header{};
    std::vector<Model> m_models{};

    Model& get_model(std::string const& name) & {
        for (size_t i = 0; i < m_models.size(); ++i) {
            if (m_models[i].m_name == name) return m_models[i];
        }
        throw std::runtime_error{"Scene: error while finding model" + name};
    }

    Model&& get_model(std::string const& name) && {
        for (size_t i = 0; i < m_models.size(); ++i) {
            if (m_models[i].m_name == name) return std::move(m_models[i]);
        }
        throw std::runtime_error{"Scene: error while finding model" + name};
    }
};

class Importor {
   public:
    std::string m_basepath{};
    Scene m_scene{};
    Importor(std::string const& basepath, std::string const& filename);

   private:
    glm::vec3 read_vec3(std::ifstream& fin);
    glm::vec2 read_vec2(std::ifstream& fin);
    glm::fquat read_fquat(std::ifstream& fin);
    void read_header(std::ifstream& fin, Header& header);
    void read_lm(std::ifstream& fin, std::vector<LocAction>& loc_markers);
    void read_lrsm(std::ifstream& fin, std::vector<LRSAction>& lrs_markers);
    void read_animation(std::ifstream& fin, std::vector<Animation>& animations);
    void read_diffuse_map(std::ifstream& fin,
                          std::vector<DiffuseMap>& diffuse_maps);
    const std::string SCENE = "SCENE";
    const std::string MODEL = "MODEL";
    const std::string MPART = "MPART";
    const std::string LM = "LM";
    const std::string LRSM = "LRSM";
    const std::string ANIMATION = "ANIMATION";
    const std::string KEYFRAME = "KEYFRAME";
    const std::string DIFFUSEMAP = "DIFFUSEMAP";
    const std::string VERTEX = "VERTEX";
};
}  // namespace Importor
#endif
