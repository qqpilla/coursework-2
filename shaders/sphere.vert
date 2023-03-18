#version 330 core

layout (location = 0) in vec3 coords;
uniform mat4 u_clip_matrix;
uniform vec3 u_cam_coords;
uniform float u_cam_distance;

out float max_alpha;

void main()
{
    gl_Position = u_clip_matrix * vec4(coords, 1.0f);

    vec3 cam2point = coords - u_cam_coords;
    float cam2point_distance_squared = pow(cam2point.x, 2) + pow(cam2point.y, 2) + pow(cam2point.z, 2);
    float max_distance_squared = pow(u_cam_distance, 2) - 1.0f; // радиус сферы = 1.0f
    float fade_distance_squared = max_distance_squared + 0.5f;
    float fade = smoothstep(max_distance_squared, fade_distance_squared, cam2point_distance_squared);

    max_alpha = fade * 0.2f + (1.0f - fade); // 0.2f на расстоянии >= fade_distance, 1.0f на <= max_distance
}