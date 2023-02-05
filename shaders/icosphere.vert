#version 330 core

layout (location = 0) in vec3 coords;
layout (location = 1) in vec3 color;
uniform mat4 u_clip_matrix;
uniform vec3 u_wireframe_color;
uniform bool u_wireframe_mode;

out vec4 v_color;

void main()
{
    gl_Position = u_clip_matrix * vec4(coords, 1.0f);
    v_color = u_wireframe_mode ? vec4(u_wireframe_color, 1.0f) : vec4(color, 1.0f);
}