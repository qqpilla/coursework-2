#include <vector>
#include "glm/vec3.hpp"

#include "icosphere.hpp"

static void CreateIcosahedron(std::vector<glm::vec3> &vertices, std::vector<unsigned int> &triangles)
{
    // phi = 1.6180339887 - число золотого сечения.
    // Длина любого ребра в икосаэдре равна a. Если радиус описанной окружности икосаэдра равен 1,
    // то a можно вычислить по следующей формуле: a = 2 / sqrt(phi * sqrt(5)).
    // Икосаэдр строится по трём ортогональным золотым прямоугольникам, в которых меньшая сторона равна a.
    // Из отношения b/a = phi: большая сторона b = a * phi.
    const float half_a = 0.5257311121f;
    const float half_b = 0.8506508083f;

    vertices =
    {
        glm::vec3(-half_a, 0.0f,  half_b), // 0
        glm::vec3( half_a, 0.0f,  half_b), // 1
        glm::vec3( half_a, 0.0f, -half_b), // 2
        glm::vec3(-half_a, 0.0f, -half_b), // 3

        glm::vec3(-half_b,  half_a, 0.0f), // 4
        glm::vec3(-half_b, -half_a, 0.0f), // 5
        glm::vec3( half_b, -half_a, 0.0f), // 6
        glm::vec3( half_b,  half_a, 0.0f), // 7

        glm::vec3(0.0f,  half_b,  half_a), // 8
        glm::vec3(0.0f, -half_b,  half_a), // 9
        glm::vec3(0.0f, -half_b, -half_a), // 10
        glm::vec3(0.0f,  half_b, -half_a)  // 11
    };

    triangles = 
    {
        // Треугольники вокруг точки 0:
        0, 1, 8,
        0, 8, 4,
        0, 4, 5,
        0, 5, 9,
        0, 9, 1,
        // Треугольники, смежные с теми, что вокруг точки 0:
        1, 7, 8,
        8, 11, 4,
        4, 3, 5,
        5, 10, 9,
        9, 6, 1,
        // Треугольнки вокруг точки 2:
        2, 10, 3,
        2, 3, 11,
        2, 11, 7,
        2, 7, 6,
        2, 6, 10,
        // Треугольники, смежные с теми, что вокруг точки 2:
        10, 5, 3,
        3, 4, 11,
        11, 8, 7,
        7, 1, 6,
        6, 9, 10
    };
}

void Icosphere::GenerateIcosphere(unsigned int depth /*= 0*/)
{
    CreateIcosahedron(_vertices, _triangles);
}