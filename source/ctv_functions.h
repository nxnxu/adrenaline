#ifndef ADRENALINE_CTV_FUNCTIONS_H
#define ADRENALINE_CTV_FUNCTIONS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "context.h"
#include "helper.h"

void ctv_G_Y(Context const&, float rx, float ry, float rz, glm::vec3 const& d_f,
             glm::vec3 const& d_r, glm::vec3& f, glm::vec3& r);
void ctv_G_XY(Context const&, float rx, float ry, float rz,
              glm::vec3 const& d_f, glm::vec3 const& d_r, glm::vec3& f,
              glm::vec3& r);

#endif  // ADRENALINE_CTV_FUNCTIONS_H
