#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/trigonometric.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "camera.hpp"

extern float delta_time;

glm::mat4 Camera::ClipSpaceMatrix()
{
    glm::vec3 position
    (
        _distance * cos(glm::radians(_yaw)) * cos(glm::radians(_pitch)),
        _distance * sin(glm::radians(_pitch)),
        _distance * sin(glm::radians(_yaw)) * cos(glm::radians(_pitch))
    );

    return _projection * glm::lookAt(position, _look_target, glm::vec3(0.0f, 0.1f, 0.0f));
}

void Camera::UpdateProjectionMatrix(unsigned int w_width, unsigned int w_height)
{
    _projection = glm::perspective(_field_of_view, float(w_width) / float(w_height), 1.0f, 20.0f);
}

void Camera::Rotate(Move yaw_direction, Move pitch_direction)
{
    _yaw -= int(yaw_direction) * _rotation_speed * delta_time;
    _pitch += int(pitch_direction) * _rotation_speed * delta_time;

    if (std::fabs(_yaw) >= 360.0f)
        _yaw -= (int(_yaw > 0.0f) * 2 - 1) * 360.0f;
    if (std::fabs(_pitch) > _max_pitch)
        _pitch = _pitch > 0.0f ? _max_pitch : -_max_pitch;
}

void Camera::Zoom(Move zoom_direction)
{
    _distance -= int(zoom_direction) * _zoom_speed * delta_time;

    _distance = std::max(_min_distance, _distance);
    _distance = std::min(_max_distance, _distance);
}