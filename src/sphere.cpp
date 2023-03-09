#include <vector>

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glad/gl.h"

#include "sphere.hpp"
#include "shader_program.hpp"

Sphere::Sphere(const std::vector<glm::vec3> &points, ShaderProgram &&shader) : _points(points), _shader(shader)
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

void Sphere::Draw() const
{
    glBindVertexArray(_VAO);
    glDrawArrays(GL_POINTS, 0, _points.size());
    glBindVertexArray(0);
}