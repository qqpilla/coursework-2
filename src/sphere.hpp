#pragma once

#include <vector>

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

#include "shader_program.hpp"

struct Rotation
{
    glm::vec3 Axis_Point; // Ось задаётся двумя точками, первая из которых - центр сферы, а вторая - Axis_Point
    float Angle;
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
};

class Sphere
{
private:
    std::vector<glm::vec3> _base_points;
    static const unsigned int _max_detail_level = 40;

    ShaderProgram _shader;

    unsigned int _VAO;
    unsigned int _coords_VBO;
    unsigned int _colors_VBO;
    unsigned int _rotations_VBO;

    void SetUpRendering();
    void UpdateCoordsVBO();
    void UpdateColorsVBO();
    void UpdateRotationsVBO();

public:
    int Detail_level;
    glm::vec3 Base_color = glm::vec3(0.2f, 0.2f, 0.2f);
    std::vector<Rotation> Rotations;

    Sphere() {}
    Sphere(const std::vector<glm::vec3> &points, ShaderProgram &&shader);
    Sphere(unsigned int level_of_detail, ShaderProgram &&shader);

    int MaxDetailLevel() const { return int(_max_detail_level); }
    
    void SetClipMatrixU(const glm::mat4 &value);
    void SetCameraCoordsU(const glm::vec3 &value);
    void SetCameraDistanceU(float value);

    void UpdateSphereShape();
    void UpdateSphereBaseColor();

    void AddRotation(Rotation &&rotation);

    void Draw() const;
};