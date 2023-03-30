#pragma once

#include <vector>

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

#include "shader_program.hpp"

class Sphere
{
private:
    std::vector<glm::vec3> _base_points;

    static const unsigned int _max_detail_level = 40;

    ShaderProgram _shader;

    unsigned int _VAO;
    unsigned int _VBO;

    void SetUpRendering();

public:
    int Detail_level;
    glm::vec3 Base_Color = glm::vec3(0.2f, 0.2f, 0.2f);

    Sphere() {}
    Sphere(const std::vector<glm::vec3> &points, ShaderProgram &&shader);
    Sphere(unsigned int level_of_detail, ShaderProgram &&shader);

    int MaxDetailLevel() const { return int(_max_detail_level); }
    
    void SetClipMatrixU(const glm::mat4 &value);
    void SetCameraCoordsU(const glm::vec3 &value);
    void SetCameraDistanceU(float value);

    void UpdateSphereShape();
    void UpdateSphereBaseColor();

    void Draw() const;
};