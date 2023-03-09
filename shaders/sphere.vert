#version 330 core

layout (location = 0) in vec3 coords;
uniform mat4 u_clip_matrix;

void main()
{
    gl_Position = u_clip_matrix * vec4(coords, 1.0f);
}