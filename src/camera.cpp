#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/trigonometric.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "camera.hpp"

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