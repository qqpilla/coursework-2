#include <vector>
#include <unordered_map>

#include "glm/vec3.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glad/gl.h"

#include "icosphere.hpp"
#include "shader_program.hpp"

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
        indices_map middle_points;

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

void Icosphere::SetUpRendering(const glm::vec3 &default_color, ShaderProgram &&shader)
{
    _shader = shader;
    _colors.resize(_vertices.size(), default_color);
    wireframe_color = default_wireframe_color;

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);

    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

    std::size_t vertices_size = _vertices.size() * sizeof(glm::vec3);
    std::size_t triangles_size = _triangles.size() * sizeof(unsigned int);
    glBufferData(GL_ARRAY_BUFFER, vertices_size * 2, _vertices.data(), GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, vertices_size, vertices_size, _colors.data());
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles_size, _triangles.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)vertices_size);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Если в программе появится больше шейдеров, то в конце функии нужно отключить шейдер,
    // и использовать его только тогда, когда требуется передать в него данные 
    // или отрисовать икосферу.
    glUseProgram(_shader.ID());

    UpdateWireframeColor();
}

void Icosphere::Render()
{
    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, _triangles.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Icosphere::UpdateWireframeColor()
{
    _shader.SetUniform3fv("u_wireframe_color", glm::value_ptr(wireframe_color));
}

void Icosphere::UseWireframeMode(bool value)
{
    _shader.SetUniform1i("u_wireframe_mode", value);
    glPolygonMode(GL_FRONT, value ? GL_LINE : GL_FILL);
}

void Icosphere::SetClipMatrix(const glm::mat4 &value)
{
    _shader.SetUniformMatrix4fv("u_clip_matrix", glm::value_ptr(value));
}