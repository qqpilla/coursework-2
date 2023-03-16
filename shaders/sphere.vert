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
    float max_distance_squared = pow(u_cam_distance, 2) - 1.0f; // sphere radius = 1.0f
    max_alpha = cam2point_distance_squared > max_distance_squared ? 0.3f : 1.0f;
}