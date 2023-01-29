#pragma once

#include <vector>
#include "glm/vec3.hpp"

class Icosphere
{
private:

    Icosphere() {}

public:
    inline static std::vector<glm::vec3> _vertices;
    inline static std::vector<unsigned int> _triangles;
    static void GenerateIcosphere(unsigned int depth = 0);
};
