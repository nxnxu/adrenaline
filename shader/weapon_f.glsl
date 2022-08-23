#version 410 core

#define MAX_MPARTS 10

flat in uint mpart_id;
in vec2 frag_uv;

out vec4 frag_color;

uniform sampler2D samplers[MAX_MPARTS];

void main()
{
    frag_color = texture(samplers[mpart_id], frag_uv);
}
