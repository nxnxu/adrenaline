#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdexcept>

Texture::Texture(std::string const& path) {
    stbi_set_flip_vertically_on_load(true);
    GLsizei w;
    GLsizei h;
    GLsizei c;
    GLvoid* data = stbi_load(path.c_str(), &w, &h, &c, 0);
    if (data) {
        glGenTextures(1, &texture_id_);
        glBindTexture(GL_TEXTURE_2D, texture_id_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (c == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
                         GL_UNSIGNED_BYTE, data);
        } else if (c == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, data);
        }
        stbi_image_free(data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        throw std::runtime_error{"Texture: " + path + " failed to load"};
    }
}

Texture::Texture(Texture&& texture) noexcept
    : texture_id_{texture.texture_id_} {
    texture.texture_id_ = 0;
}

Texture::~Texture() { glDeleteTextures(1, &texture_id_); }

void Texture::bind_to_texture_unit(GLenum unit) const noexcept {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
}
