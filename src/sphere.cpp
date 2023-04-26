#include <vector>

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/mat3x3.hpp"
#include "glm/trigonometric.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glad/gl.h"

#include "sphere.hpp"
#include "shader_program.hpp"

void CreateUvSphere(float radius, unsigned int detail_level, std::vector<glm::vec3> &points_container);

Sphere::Sphere(const std::vector<glm::vec3> &points, ShaderProgram &&shader) : _base_points(points), _shader(shader), Detail_level(0)
{
    _rotations.resize(Rotation::MaxRotations());
    unsigned int ind = 0;
    for (unsigned int depth = 1; depth < Rotation::Max_depth; depth++)
    {
        unsigned int local_rotations_num = pow(Rotation::Max_children, depth);
        for (int i = 0; i < local_rotations_num; i++)
        {
            _rotations[ind].second = ind - i + i * Rotation::Max_children + local_rotations_num;
            ind++;
        }
    }
    // Повороты на максимальной глубине не имеют потомков
    for (; ind < _rotations.size(); ind++)
        _rotations[ind].second = -1;

    SetUpRendering();
}

Sphere::Sphere(unsigned int level_of_detail, ShaderProgram &&shader) : Detail_level(level_of_detail), _shader(shader)
{
    Detail_level = std::min(Detail_level, int(_max_detail_level));
    Detail_level = std::max(Detail_level, 1);

    _rotations.resize(Rotation::MaxRotations());
    unsigned int ind = 0;
    for (unsigned int depth = 1; depth < Rotation::Max_depth; depth++)
    {
        unsigned int local_rotations_num = pow(Rotation::Max_children, depth);
        for (int i = 0; i < local_rotations_num; i++)
        {
            _rotations[ind].second = ind - i + i * Rotation::Max_children + local_rotations_num;
            ind++;
        }
    }
    // Повороты на максимальной глубине не имеют потомков
    for (; ind < _rotations.size(); ind++)
        _rotations[ind].second = -1;

    CreateUvSphere(1.0f, Detail_level, _base_points);
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
    glUseProgram(_shader.ID());

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_coords_VBO);
    glGenBuffers(1, &_colors_VBO);
    glGenBuffers(1, &_rotations_VBO);
    glGenBuffers(1, &_actives_VBO);

    glBindVertexArray(_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, _coords_VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    unsigned int max_spheres = _rotations.size() + 1;

    glBindBuffer(GL_ARRAY_BUFFER, _colors_VBO);
    glBufferData(GL_ARRAY_BUFFER, max_spheres * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    std::vector<glm::mat3> matrices(max_spheres, glm::mat3(1.0f));
    glBindBuffer(GL_ARRAY_BUFFER, _rotations_VBO);
    glBufferData(GL_ARRAY_BUFFER, max_spheres * sizeof(glm::mat3), matrices.data(), GL_STATIC_DRAW);
    for (int i = 0; i < 3; i++)
    {
        glVertexAttribPointer(2 + i, 3, GL_FLOAT, GL_FALSE, sizeof(glm::mat3), (void*)(i*sizeof(glm::vec3)));
        glEnableVertexAttribArray(2 + i);
        glVertexAttribDivisor(2 + i, 1);
    }

    std::vector<int> actives = std::vector<int>(max_spheres, 0); actives[0] = 1;
    glBindBuffer(GL_ARRAY_BUFFER, _actives_VBO);
    glBufferData(GL_ARRAY_BUFFER, max_spheres * sizeof(int), actives.data(), GL_STATIC_DRAW);
    glVertexAttribIPointer(5, 1, GL_INT, 0, 0);
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    UpdateCoordsVBO();
    UpdateSphereBaseColor();
}

void Sphere::UpdateCoordsVBO()
{
    std::size_t points_size = _base_points.size() * sizeof(glm::vec3);

    glBindBuffer(GL_ARRAY_BUFFER, _coords_VBO);
    glBufferData(GL_ARRAY_BUFFER, points_size, _base_points.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Sphere::PutDataIntoVBO(unsigned int &VBO, std::size_t offset, std::size_t size, const void* data)
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Sphere::SetClipMatrixU(const glm::mat4 &value)
{
    _shader.SetUniformMatrix4fv("u_clip_matrix", glm::value_ptr(value));
}

void Sphere::SetCameraCoordsU(const glm::vec3 &value)
{
    _shader.SetUniform3fv("u_cam_coords", glm::value_ptr(value));
}

void Sphere::SetCameraDistanceU(float value)
{
    _shader.SetUniform1f("u_cam_distance", value);
}

void Sphere::UpdateSphereShape()
{
    CreateUvSphere(1.0f, Detail_level, _base_points);
    UpdateCoordsVBO();
}

void Sphere::UpdateSphereBaseColor()
{
    glBindBuffer(GL_ARRAY_BUFFER, _colors_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3), glm::value_ptr(Base_color));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Sphere::AddRotation(unsigned int ind)
{
    Rotation* rotation = &_rotations[ind].first;

    rotation->_is_active = true;
    int one = 1;
    PutDataIntoVBO(_rotations_VBO, (ind + 1) * sizeof(glm::mat3), sizeof(glm::mat3), glm::value_ptr(rotation->_parent_matrix));
    PutDataIntoVBO(_actives_VBO, (ind + 1) * sizeof(int), sizeof(int), &one);
    UpdateChildRotations(ind, rotation->_parent_matrix);
}

void Sphere::UpdateRotation(unsigned int ind, bool rotation_changed, bool color_changed)
{
    const Rotation* rotation = &_rotations[ind].first;

    if (rotation_changed)
    {
        glm::mat3 rotation_matrix = glm::mat3(glm::rotate(glm::mat4(rotation->_parent_matrix), rotation->Angle, rotation->Axis));
        PutDataIntoVBO(_rotations_VBO, (ind + 1) * sizeof(glm::mat3), sizeof(glm::mat3), glm::value_ptr(rotation_matrix));
        UpdateChildRotations(ind, rotation_matrix);
    }

    if (color_changed)
        PutDataIntoVBO(_colors_VBO, (ind + 1) * sizeof(glm::vec3), sizeof(glm::vec3), glm::value_ptr(rotation->Color));
}

void Sphere::UpdateChildRotations(unsigned int parent_ind, const glm::mat3 &parent_matrix)
{
    unsigned int first_child_ind = _rotations[parent_ind].second;
    if (first_child_ind == -1)
        return;

    for (unsigned int i = first_child_ind; i < first_child_ind + Rotation::Max_children; i++)
    {
        _rotations[i].first._parent_matrix = parent_matrix;
        if (_rotations[i].first._is_active)
            UpdateRotation(i, true, false);
    }
}

void Sphere::Draw() const
{
    glBindVertexArray(_VAO);
    glDrawArraysInstanced(GL_POINTS, 0, _base_points.size(), _rotations.size() + 1);
    glBindVertexArray(0);
}