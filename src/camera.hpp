#pragma once

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

class Camera
{
private:
    constexpr static glm::vec3 _look_target = glm::vec3(0.0f, 0.0f, 0.0f);

    // Расстояние до _look_target
    inline static float _distance = 5.0f;
    constexpr static float _min_distance = 2.2f;
    constexpr static float _max_distance = 8.0f;

    inline static float _yaw = 0.0f;   // Угол поворота вокруг оси Y
    inline static float _pitch = 0.0f; // Угол поворота вокруг оси X
    constexpr static float _max_pitch = 50.0f;

    constexpr static float _rotation_speed = 25.0f;
    constexpr static float _zoom_speed = 4.5f;

    inline static glm::mat4 _projection;
    constexpr static float _field_of_view = 45.0f;

    Camera() {}

public:
    enum class Move
    {
        RIGHT = 1, LEFT = -1,
        UP = 1, DOWN = -1,
        IN = 1, OUT = -1,
        STAY = 0
    };

    static glm::mat4 ClipSpaceMatrix();
    static void UpdateProjectionMatrix(unsigned int w_width, unsigned int w_height);
    static void Rotate(Move yaw_direction, Move pitch_direction);
    static void Zoom(Move zoom_direction);
};