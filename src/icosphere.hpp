#pragma once

#include <vector>

#include "glm/vec3.hpp"

#include "shader_program.hpp"

class Icosphere
{
private:
    inline static std::vector<glm::vec3> _vertices;
    inline static std::vector<glm::vec3> _colors;
    inline static std::vector<unsigned int> _triangles;

    inline static glm::vec3 wireframe_color = glm::vec3(1.0f, 0.0f, 0.0f);

    inline static ShaderProgram _shader;

    inline static unsigned int _VAO;
    inline static unsigned int _VBO;
    inline static unsigned int _EBO;

    Icosphere() {}

public:

    static void GenerateIcosphere(unsigned int depth = 0);
    static void SetUpRendering(const glm::vec3 &default_color, ShaderProgram &&shader);
    static void Render();

    static void UpdateWireframeColor();
    static void UseWireframeMode(bool value);
    static void SetClipMatrix(const glm::mat4 &value);
};
