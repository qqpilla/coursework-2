#version 330 core

in float max_alpha;
out vec4 color;

void main()
{
    float distance_squared = dot(gl_PointCoord - vec2(0.5f), gl_PointCoord - vec2(0.5f));
    vec3 red = vec3(1.0f, 0.0f, 0.0f);

    color = mix(vec4(red, max_alpha), vec4(red, 0.0f), smoothstep(0.16f, 0.25f, distance_squared));
}