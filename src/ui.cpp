#include <vector>

#include "glad/gl.h"
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/vec3.hpp"
#include "glm/mat3x3.hpp"

#include "ui.hpp"
#include "sphere.hpp"

UI::UI(Sphere* sphere, GLFWwindow *window, const char *glsl_version) : _sphere(sphere)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    _rotations_points.resize(Rotation::MaxRotations());
    std::fill(_rotations_points.begin(), _rotations_points.end(), _sphere->BasePoints());
}

void UI::Die()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

void UI::BeginFrame()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void UI::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::DrawPropertiesWindow()
{
    // ImGui::ShowDemoWindow();
    ImGui::SetNextWindowSizeConstraints({400, -1}, {INFINITY, -1});
    if (!ImGui::Begin("##UiWindow", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("SPHERE PROPERTIES:");
    if(_sphere->Detail_level && ImGui::SliderInt("Level of Detail", &(_sphere->Detail_level), 1, _sphere->MaxDetailLevel()))
    {
        _sphere->UpdateSphereShape();
        UpdateLevelOfDetail();
    }
    if (ImGui::ColorEdit3("Base Color", glm::value_ptr(_sphere->Base_color)))
        _sphere->UpdateSphereBaseColor();
    if (ImGui::Checkbox("Visible", &_sphere->Is_visible))
        _sphere->ChangeVisibility(ImGui::GetIO().KeyCtrl);

    ImGui::Separator();
    ImGui::Text("ROTATIONS:");

    for (unsigned int i = 0; i < Rotation::Max_children; i++)
        DisplayRotationNode(i);

    ImGui::End();
}

void UI::DisplayRotationNode(unsigned int ind)
{
    std::string id = "##Node" + std::to_string(ind);
    if (_sphere->Rotations()[ind].second != -1)
    {
        bool opened = ImGui::TreeNodeEx(id.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);
        TryApplyChanges(DisplayRotationContent(_sphere->RotationByIndex(ind), std::to_string(ind)), ind);

        if (opened)
        {
            for (unsigned int i = 0; i < Rotation::Max_children; i++)
                DisplayRotationNode(_sphere->Rotations()[ind].second + i);
            ImGui::TreePop();
        }
    }
    else
    {
        ImGui::Bullet();
        TryApplyChanges(DisplayRotationContent(_sphere->RotationByIndex(ind), std::to_string(ind)), ind);
    }
}

static float GetPeriodicValue(float value, float period)
{
    if (std::fabs(value) < period)
        return value;

    return value > 0 ? value - period * std::floor(value / period)
                     : value + period * std::floor(std::fabs(value) / period);
}

std::tuple<bool, bool, std::pair<bool, bool>> UI::DisplayRotationContent(Rotation &rotation, const std::string &id)
{
    std::tuple<bool, bool, std::pair<bool, bool>> changed = {false, false, {false, false}};

    std::string angle_label = "Angle##" + id;
    std::string axis_label = "Axis Vector##" + id;
    std::string color_label = "Color##" + id;
    std::string visible_label = "Visible##" + id; 

    ImGui::SameLine(); 
    if (std::get<0>(changed) = ImGui::InputFloat(angle_label.c_str(), &rotation.Angle, 0.1f, 1.0f, "%.2f"))
        rotation.Angle = GetPeriodicValue(rotation.Angle, 360.0f);

    ImGui::SameLine(); 
    std::get<0>(changed) = ImGui::InputFloat3(axis_label.c_str(), glm::value_ptr(rotation.Axis), "%.2f") || std::get<0>(changed);

    ImGui::SameLine(); 
    std::get<1>(changed) = ImGui::ColorEdit3(color_label.c_str(), glm::value_ptr(rotation.Color), ImGuiColorEditFlags_NoInputs);
        
    ImGui::SameLine();
    std::get<2>(changed).first = ImGui::Checkbox(visible_label.c_str(), &rotation.Is_visible);
    std::get<2>(changed).second = ImGui::GetIO().KeyCtrl;

    return changed;
}

void UI::TryApplyChanges(const std::tuple<bool, bool, std::pair<bool, bool>> &changes, unsigned int rotation_ind)
{
    if (!std::get<0>(changes) && !std::get<1>(changes) && !std::get<2>(changes).first)
        return;

    _sphere->UpdateRotation(rotation_ind, std::get<0>(changes), std::get<1>(changes), std::get<2>(changes));
    if (std::get<0>(changes))
        UpdateRotationPoints(rotation_ind);
}

void UI::UpdateLevelOfDetail()
{
    std::fill(_rotations_points.begin(), _rotations_points.end(), _sphere->BasePoints());
    for (int i = 0; i < _rotations_points.size(); i++)
    {
        Rotation rotation = _sphere->RotationByIndex(i);
        
        if (rotation.Angle == 0.0f && rotation.ParentMatrix() == glm::mat3(1.0f))
        {
            continue;
        }
        else
        {
            glm::mat3 rotation_matrix = glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(rotation.Angle), glm::normalize(rotation.Axis))) * rotation.ParentMatrix();
            for (int j = 0; j < _rotations_points[i].size(); j++)
                _rotations_points[i][j] = rotation_matrix * _sphere->BasePoints()[j];
        }
    }
}

void UI::UpdateRotationPoints(unsigned int ind)
{
    Rotation rotation = _sphere->RotationByIndex(ind);
    glm::mat3 rotation_matrix = glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(rotation.Angle), glm::normalize(rotation.Axis))) * rotation.ParentMatrix();
    for (int i = 0; i < _rotations_points[ind].size(); i++)
        _rotations_points[ind][i] = rotation_matrix * _sphere->BasePoints()[i];

    if (_sphere->Rotations()[ind].second != -1)
        for (int i = 0; i < Rotation::Max_children; i++)
            UpdateRotationPoints(_sphere->Rotations()[ind].second + i);
}