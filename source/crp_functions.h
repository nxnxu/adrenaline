#ifndef ADRENALINE_CRP_FUNCTIONS_H
#define ADRENALINE_CRP_FUNCTIONS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "context.h"
#include "helper.h"

void crp_G_XY(Context const& c, glm::mat4& r, float& rx, float& ry, float& rz,
              float x, float y, float z);

#endif  // ADRENALINE_CRP_FUNCTIONS_H
