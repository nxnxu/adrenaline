#ifndef MOUSE_CONTROLLER_H
#define MOUSE_CONTROLLER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "context.h"
#include "helper.h"
#include "input.h"

class MouseController {
   public:
    MouseController() = default;

    void tick(Context const& context) noexcept;

    float get_x_dt() const noexcept { return rotate_x_dt; }
    float get_y_dt() const noexcept { return rotate_y_dt; }

   private:
    float rotate_x_dt{};
    float rotate_y_dt{};
};

#endif  // MOUSE_CONTROLLER_H
