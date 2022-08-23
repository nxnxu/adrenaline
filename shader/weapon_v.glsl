#version 410 core

#define MAX_MPARTS 10

layout (location = 0) in uint id;
layout (location = 1) in vec3 vertex_pos;
layout (location = 2) in vec2 vertex_uv;
layout (location = 3) in vec3 vertex_nor;

flat out uint mpart_id;
out vec2 frag_uv;

uniform mat4 mpart_trans[MAX_MPARTS];
uniform mat4 vp;
uniform mat4 pTrans;

void main () {
    gl_Position = vp * pTrans * mpart_trans[id] * vec4(vertex_pos, 1.0f);
    frag_uv   = vertex_uv;
    mpart_id  = id;
}
