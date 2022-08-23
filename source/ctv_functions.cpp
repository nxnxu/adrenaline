#include "ctv_functions.h"

using glm::mat4;
using glm::radians;
using glm::rotate;
using glm::vec3;
using glm::vec4;

void ctv_G_Y(Context const&, float rx, float ry, float rz, vec3 const& d_f,
             vec3 const& d_r, vec3& f, vec3& r) {
    (void)rx;
    (void)rz;
    vec3 y_axis = vec3{0.0f, 1.0f, 0.0f};
    mat4 y_mat = rotate(mat4{1.0f}, radians(ry), y_axis);

    f = y_mat * vec4(d_f, 0.0f);
    r = y_mat * vec4(d_r, 0.0f);
}

void ctv_G_XY(Context const&, float rx, float ry, float rz, vec3 const& d_f,
              vec3 const& d_r, vec3& f, vec3& r) {
    (void)rz;
    vec3 x_axis = vec3{1.0f, 0.0f, 0.0f};
    vec3 y_axis = vec3{0.0f, 1.0f, 0.0f};
    mat4 x_mat = rotate(mat4{1.0f}, radians(rx), x_axis);
    mat4 y_mat = rotate(mat4{1.0f}, radians(ry), y_axis);

    f = glm::normalize(y_mat * x_mat * vec4(d_f, 0.0f));
    r = glm::normalize(y_mat * x_mat * vec4(d_r, 0.0f));
}