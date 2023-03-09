#pragma once

#include <vector>

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

#include "shader_program.hpp"

class Sphere
{
private:
    std::vector<glm::vec3> _points;
    ShaderProgram _shader;

    unsigned int _VAO;
    unsigned int _VBO;

public:
    Sphere() {}
    Sphere(const std::vector<glm::vec3> &points, ShaderProgram &&shader);
    
    void SetClipMatrix(const glm::mat4 &value);
    void Draw() const;
};