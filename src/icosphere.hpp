#pragma once

#include <vector>
#include "glm/vec3.hpp"

class Icosphere
{
private:
    inline static std::vector<glm::vec3> _vertices;
    inline static std::vector<unsigned int> _triangles;

    Icosphere() {}

public:
    static void GenerateIcosphere(unsigned int depth = 0);
};
