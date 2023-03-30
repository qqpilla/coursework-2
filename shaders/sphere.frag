#version 330 core

in float max_alpha;
uniform vec3 u_base_color;

out vec4 color;

void main()
{
    float distance_squared = dot(gl_PointCoord - vec2(0.5f), gl_PointCoord - vec2(0.5f));

    color = mix(vec4(u_base_color, max_alpha), vec4(u_base_color, 0.0f), smoothstep(0.16f, 0.25f, distance_squared));
}