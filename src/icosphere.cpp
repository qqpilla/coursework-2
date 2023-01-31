#include <vector>
#include <unordered_map>
#include "glm/vec3.hpp"
#include "glm/geometric.hpp"

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
        glm::vec3(-half_a, 0.0f, half_b),  // 0
        glm::vec3(half_a, 0.0f, half_b),   // 1
        glm::vec3(half_a, 0.0f, -half_b),  // 2
        glm::vec3(-half_a, 0.0f, -half_b), // 3

        glm::vec3(-half_b, half_a, 0.0f),  // 4
        glm::vec3(-half_b, -half_a, 0.0f), // 5
        glm::vec3(half_b, -half_a, 0.0f),  // 6
        glm::vec3(half_b, half_a, 0.0f),   // 7

        glm::vec3(0.0f, half_b, half_a),   // 8
        glm::vec3(0.0f, -half_b, half_a),  // 9
        glm::vec3(0.0f, -half_b, -half_a), // 10
        glm::vec3(0.0f, half_b, -half_a)   // 11
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

static glm::vec3 NormalizedMiddlePoint(const glm::vec3 &point_a, const glm::vec3 &point_b)
{
    return glm::normalize((point_a + point_b) / 2.0f);
}

namespace std
{
    template <typename T1, typename T2>
    struct hash<std::pair<T1, T2>>
    {
        std::size_t operator()(const std::pair<T1, T2> &p) const
        {
            std::size_t h1 = std::hash<T1>()(p.first);
            std::size_t h2 = std::hash<T2>()(p.second);

            return h1 ^ h2;
        }
    };
}

using indices_map = std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>;

static unsigned int FindOrPushMiddlePoint(indices_map &middle_points, std::vector<glm::vec3> &vertices, 
    unsigned int a_ind, unsigned int b_ind)
{
    std::pair<unsigned int, unsigned int> key = {std::min(a_ind, b_ind), std::max(a_ind, b_ind)};

    if (middle_points.count(key) > 0)
        return middle_points[key];

    vertices.push_back(NormalizedMiddlePoint(vertices[a_ind], vertices[b_ind]));
    middle_points[key] = vertices.size() - 1;
    return vertices.size() - 1;
}

static void PushTriangle(std::vector<unsigned int> &triangles, unsigned int a_ind, unsigned int b_ind, unsigned int c_ind)
{
    triangles.push_back(a_ind);
    triangles.push_back(b_ind);
    triangles.push_back(c_ind);
}

void Icosphere::GenerateIcosphere(unsigned int depth /*= 0*/)
{
    CreateIcosahedron(_vertices, _triangles);

    for (int i = 0; i < depth; i++)
    {
        std::vector<unsigned int> new_triangles;
        std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int> middle_points;

        for (int j = 0; j < _triangles.size(); j += 3)
        {
            unsigned int a_ind = FindOrPushMiddlePoint(middle_points, _vertices, _triangles[j], _triangles[j + 1]);
            unsigned int b_ind = FindOrPushMiddlePoint(middle_points, _vertices, _triangles[j + 1], _triangles[j + 2]);
            unsigned int c_ind = FindOrPushMiddlePoint(middle_points, _vertices, _triangles[j + 2], _triangles[j]);

            PushTriangle(new_triangles, a_ind, _triangles[j + 1], b_ind);
            PushTriangle(new_triangles, b_ind, _triangles[j + 2], c_ind);
            PushTriangle(new_triangles, c_ind, _triangles[j], a_ind);
            PushTriangle(new_triangles, a_ind, b_ind, c_ind);
        }

        _triangles = new_triangles;
    }
}