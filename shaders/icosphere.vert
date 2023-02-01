#version 330 core

layout (location = 0) in vec3 coords;
layout (location = 1) in vec3 color;

out vec4 v_color;

void main()
{
    gl_Position = vec4(coords, 1.0f);
    v_color = vec4(color, 1.0f);
}