#version 330 core

layout (location = 0) in uint ndc;
layout (location = 1) in uint psz;
layout (location = 2) in vec3 pos;
layout (location = 3) in vec4 col;

uniform mat4 vp;
uniform mat4 p_proj;

out vec4 vertex_color;

void main()
{
    bool NDC = bool(ndc);
    if (NDC) {
        gl_Position = vec4(pos, 1.0);
    } else {
        gl_Position = vp * vec4(pos, 1.0);
    }

    gl_PointSize = psz;
    vertex_color = col;
}