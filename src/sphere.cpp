#include <vector>

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/trigonometric.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glad/gl.h"

#include "sphere.hpp"
#include "shader_program.hpp"

void CreateUvSphere(float radius, unsigned int detail_level, std::vector<glm::vec3> &points_container);

Sphere::Sphere(const std::vector<glm::vec3> &points, ShaderProgram &&shader) : _points(points), _shader(shader), Detail_level(0)
{
    SetUpRendering();
}

Sphere::Sphere(unsigned int level_of_detail, ShaderProgram &&shader) : Detail_level(level_of_detail), _shader(shader)
{
    Detail_level = std::min(Detail_level, int(_max_detail_level));
    Detail_level = std::max(Detail_level, 1);

    CreateUvSphere(1.0f, Detail_level, _points);
    SetUpRendering();
}

void CreateUvSphere(float radius, unsigned int detail_level, std::vector<glm::vec3> &points_container)
{
    unsigned int v_segments_count = detail_level + 2;
    unsigned int h_segments_count = detail_level + 1;
    float v_step = 180.0f / h_segments_count;
    float h_step = 360.0f / v_segments_count;
    unsigned int points_count = v_segments_count * (h_segments_count - 1) + 2;

    points_container.resize(points_count);

    // Верхняя точка сферы
    unsigned int ind = 0;
    points_container[ind++] = {0.0f, radius, 0.0f};

    for (int i = 1; i < h_segments_count; i++)
    {
        float v_angle = -v_step * i + 90.0f;
        for (int j = 0; j < v_segments_count; j++)
        {
            float h_angle = h_step * j;

            points_container[ind++] =
            {
                radius * glm::cos(glm::radians(h_angle)) * glm::cos(glm::radians(v_angle)),
                radius * glm::sin(glm::radians(v_angle)),
                radius * glm::sin(glm::radians(h_angle)) * glm::cos(glm::radians(v_angle))
            };
        }
    }

    // Нижняя точка сферы
    points_container[ind] = {0.0f, -radius, 0.0f};
}

void Sphere::SetUpRendering()
{
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);

    std::size_t points_size = _points.size() * sizeof(glm::vec3);
    glBufferData(GL_ARRAY_BUFFER, points_size, _points.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(_shader.ID());
}

void Sphere::SetClipMatrix(const glm::mat4 &value)
{
    _shader.SetUniformMatrix4fv("u_clip_matrix", glm::value_ptr(value));
}

void Sphere::UpdateSphereShape()
{
    CreateUvSphere(1.0f, Detail_level, _points);
    SetUpRendering();   
}

void Sphere::Draw() const
{
    glBindVertexArray(_VAO);
    glDrawArrays(GL_POINTS, 0, _points.size());
    glBindVertexArray(0);
}