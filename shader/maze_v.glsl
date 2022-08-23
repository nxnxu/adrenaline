#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 tra;

uniform mat4 vp;

void main()
{   
    vec3 position = vec3(pos.x, pos.y, pos.z);
    gl_Position = vp * vec4(position + tra, 1.0);
}