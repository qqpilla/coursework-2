#version 330 core

in vec4 v_color;
out vec4 f_color;

void main()
{
    float distance_squared = dot(gl_PointCoord - vec2(0.5f), gl_PointCoord - vec2(0.5f));
    f_color = mix(v_color, vec4(v_color.rgb, 0.0f), smoothstep(0.16f, 0.25f, distance_squared));
}