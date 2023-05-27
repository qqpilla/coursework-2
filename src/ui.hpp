#pragma once

#include <vector>

#include "glad/gl.h"
#include "glm/vec3.hpp"

#include "sphere.hpp"

class UI
{
private:
    Sphere* _sphere;
    std::vector<std::vector<glm::vec3>> _rotations_points;

    std::tuple<bool, bool, std::pair<bool, bool>> DisplayRotationContent(Rotation &rotation, const std::string &id);
    void DisplayRotationNode(unsigned int ind);
    void TryApplyChanges(const std::tuple<bool, bool, std::pair<bool, bool>> &changes, unsigned int rotation_ind);

    void UpdateLevelOfDetail();
    void UpdateRotationPoints(unsigned int ind);

public:
    UI (Sphere *sphere, GLFWwindow *window, const char *glsl_version);
    void Die();

    void BeginFrame();
    void EndFrame();
    void DrawPropertiesWindow();
    void DrawRotationsResultsWindow();
};