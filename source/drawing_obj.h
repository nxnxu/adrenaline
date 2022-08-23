#ifndef ADRENALINE_DRAWING_OBJ_H
#define ADRENALINE_DRAWING_OBJ_H

#include <glad/glad.h>

#include "ParticleManager.h"
#include "PrimitiveManager.h"

struct DrawingObj {
    PrimitiveManager pm_;
    PrimitiveManager lm_;
    PrimitiveManager no_depth_pm_;
    PrimitiveManager no_depth_lm_;
    ParticleManager particle_manager_;

    DrawingObj(size_t max_size)
        : pm_{max_size, GL_POINTS},
          lm_{max_size, GL_LINES},
          no_depth_pm_{max_size, GL_POINTS},
          no_depth_lm_{max_size, GL_LINES},
          particle_manager_{max_size} {}

    void tick(Context const& context) {
        particle_manager_.tick(context, pm_);
        pm_.tick(context);
        lm_.tick(context);
        no_depth_pm_.tick(context);
        no_depth_lm_.tick(context);
    }
};

#endif  // ADRENALINE_DRAWING_OBJ_H
