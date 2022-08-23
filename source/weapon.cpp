#include "weapon.h"

using glm::vec3;
using glm::vec4;

void SQUARE_CROSSHAIR(Context const &context, DrawingObj &draw_obj) {
    float hw = 1.0f * context.m_window_width / 2.0f;
    float hh = 1.0f * context.m_window_height / 2.0f;

    float xlen = 0.05f;
    float ylen = xlen * hw / hh;

    PrimitiveManager::Vertex a{true, 2, vec3{-xlen, -ylen, 0.0f},
                               vec4{1.0f, 0.0f, 0.0f, 1.0f}};
    PrimitiveManager::Vertex b{true, 2, vec3{xlen, -ylen, 0.0f},
                               vec4{1.0f, 0.0f, 0.0f, 1.0f}};
    PrimitiveManager::Vertex c{true, 2, vec3{xlen, ylen, 0.0f},
                               vec4{1.0f, 0.0f, 0.0f, 1.0f}};
    PrimitiveManager::Vertex d{true, 2, vec3{-xlen, ylen, 0.0f},
                               vec4{1.0f, 0.0f, 0.0f, 1.0f}};

    PrimitiveManager::Vertex lines[] = {a, b, b, c, c, d, d, a};
    draw_obj.no_depth_lm_.copy_to_temparary_buffer(lines, 8);
}

void DOT_CROSSHAIR(Context const &context, DrawingObj &draw_obj) {
    (void)context;
    static PrimitiveManager::Vertex crosshair{true, 10, vec3{0.0f},
                                              vec4{1.0f, 0.0f, 0.0f, 1.0f}};
    draw_obj.no_depth_pm_.copy_to_temparary_buffer(&crosshair, 1);
}