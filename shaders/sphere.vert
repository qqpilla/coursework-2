#version 330 core

layout (location = 0) in vec3 coords;
layout (location = 1) in vec3 color;
layout (location = 2) in vec4 rotation;
uniform mat4 u_clip_matrix;
uniform vec3 u_cam_coords;
uniform float u_cam_distance;

out vec4 v_color;

const float min_alpha = 0.2f;
const float sphere_radius = 1.0f;
const float max_points_size = 12.0f;
const float max_cam_distance_squared = 100.0f;

void main()
{
    gl_Position = u_clip_matrix * vec4(coords, 1.0f);

    vec3 cam2point = coords - u_cam_coords;
    float cam2point_distance_squared = pow(cam2point.x, 2) + pow(cam2point.y, 2) + pow(cam2point.z, 2);
    float max_distance_squared = pow(u_cam_distance, 2) - sphere_radius;
    float fade_distance_squared = max_distance_squared + 0.5f;

    float scale = 1.0f - cam2point_distance_squared / max_cam_distance_squared;
    gl_PointSize = scale * max_points_size;

    float fade = smoothstep(max_distance_squared, fade_distance_squared, cam2point_distance_squared);
    float max_alpha = fade * min_alpha + (1.0f - fade); // min_alpha на расстоянии >= fade_distance, 1.0f на <= max_distance
    v_color = vec4(color, max_alpha);
}