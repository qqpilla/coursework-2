#pragma once

#include <vector>

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/mat3x3.hpp"

#include "shader_program.hpp"

struct Rotation
{
public:
    float Angle = 0.0f;
    // Ось - это вектор, проходящий через центр сферы (начало координат) и вторую точку, 
    // координаты которой и будут координатами вектора, поэтому назовём её просто Axis
    glm::vec3 Axis = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 Color;

    static const unsigned int Max_depth = 2;
    static const unsigned int Max_children = 3;

    static unsigned int MaxRotations()
    {
        unsigned int m_rotations = 0;
        for (int depth = 1; depth <= Max_depth; depth++)
            m_rotations += pow(Max_children, depth);
        return m_rotations;
    }

    bool IsActive() const { return _is_active; }

    friend class Sphere;

private:

    glm::mat3 _parent_matrix = glm::mat3(1.0f);
    bool _is_active = false;
};

class Sphere
{
private:
    std::vector<glm::vec3> _base_points;
    std::vector<std::pair<Rotation, int>> _rotations; // int - индекс первого потомка поворота

    static const unsigned int _max_detail_level = 40;

    ShaderProgram _shader;

    unsigned int _VAO;
    unsigned int _coords_VBO;    // Содержит координаты _base_points
    unsigned int _colors_VBO;    // Содержит цвет сферы и цвета всех поворотов
    unsigned int _rotations_VBO; // Содержит матрицы 3x3, задающие повороты (для сферы содержит единичную матрицу в качестве матрицы поворота)
    unsigned int _actives_VBO;   // Содержит значения члена _is_active поворотов (для сферы это всегда 1)


    void SetUpRendering();
    void UpdateCoordsVBO();
    void PutDataIntoVBO(unsigned int &VBO, std::size_t offset, std::size_t size, const void* data);

    void UpdateChildRotations(unsigned int parent_ind, const glm::mat3 &parent_matrix);

public:
    int Detail_level;
    glm::vec3 Base_color = glm::vec3(0.2f, 0.2f, 0.2f);

    Sphere() {}
    Sphere(const std::vector<glm::vec3> &points, ShaderProgram &&shader);
    Sphere(unsigned int level_of_detail, ShaderProgram &&shader);

    const std::vector<std::pair<Rotation, int>>& Rotations() const { return _rotations; }
    Rotation& RotationByIndex(unsigned int ind) { return _rotations[ind].first; } // Позволяет изменить поворот, но не структуру вектора _rotations
    int MaxDetailLevel() const { return int(_max_detail_level); }
    
    void SetClipMatrixU(const glm::mat4 &value);
    void SetCameraCoordsU(const glm::vec3 &value);
    void SetCameraDistanceU(float value);

    void UpdateSphereShape();
    void UpdateSphereBaseColor();

    void AddRotation(unsigned int ind);
    void UpdateRotation(unsigned int ind, bool rotation_changed, bool color_changed);

    void Draw() const;
};