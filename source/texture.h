#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <stb_image.h>

#include <string>

class Texture {
   public:
    explicit Texture(std::string const& path);
    Texture(Texture const&) = delete;
    Texture(Texture&&) noexcept;
    ~Texture();
    void bind_to_texture_unit(GLenum unit) const noexcept;

   private:
    GLuint texture_id_{};
};

#endif  // TEXTURE_H
