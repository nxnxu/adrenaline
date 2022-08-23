#include "crp_functions.h"

using glm::mat4;
using glm::radians;
using glm::rotate;
using glm::vec3;
using glm::vec4;

void crp_G_XY(Context const& c, mat4& r, float& rx, float& ry, float& rz,
              float x, float y, float z) {
    (void)c;
    (void)rz;
    (void)z;
    rx = clamp(rx + x, -90.0f, 90.0f);
    ry = ry + y;

    vec3 x_axis = vec3{1.0f, 0.0f, 0.0f};
    vec3 y_axis = vec3{0.0f, 1.0f, 0.0f};

    mat4 x_rot = rotate(mat4{1.0f}, radians(rx), x_axis);
    mat4 y_rot = rotate(mat4{1.0f}, radians(ry), y_axis);

    r = y_rot * x_rot;
}