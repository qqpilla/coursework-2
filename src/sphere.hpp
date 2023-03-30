#pragma once

#include <vector>

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

#include "shader_program.hpp"

class Sphere
{
private:
    std::vector<glm::vec3> _points;

    static const unsigned int _max_detail_level = 40;
    inline static const glm::vec3 _default_base_color = glm::vec3(0.2f, 0.2f, 0.2f);

    ShaderProgram _shader;

    unsigned int _VAO;
    unsigned int _VBO;

    void SetUpRendering();

public:
    int Detail_level;

    Sphere() {}
    Sphere(const std::vector<glm::vec3> &points, ShaderProgram &&shader);
    Sphere(unsigned int level_of_detail, ShaderProgram &&shader);

    int MaxDetailLevel() const { return int(_max_detail_level); }
    
    void SetClipMatrixU(const glm::mat4 &value);
    void SetCameraCoordsU(const glm::vec3 &value);
    void SetCameraDistanceU(float value);

    void UpdateSphereShape();

    void Draw() const;
};