#include <iostream>

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/vec2.hpp"
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

#include "sphere.hpp"
#include "camera.hpp"

static const char *glsl_version = "#version 330";

static Sphere sphere;

static bool clip_update_needed = true;
static glm::vec2 last_cursor_pos;

float delta_time = 0.0f;

void ErrorCallback(int error_code, const char *message)
{
    std::cout << "ERROR: " << message << "\nERROR CODE: " << error_code << std::endl;
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void ScrollCallback(GLFWwindow *window, double x_offset, double y_offset)
{
    if (y_offset == 0)
        return;

    Camera::Zoom(y_offset > 0 ? int(Camera::Move::IN) : int(Camera::Move::OUT));
    sphere.SetCameraDistanceU(Camera::Distance());
    clip_update_needed = true;
}

void CursorPosCallback(GLFWwindow *window, double x_pos, double y_pos)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        glm::vec2 move(-(x_pos - last_cursor_pos.x) * Camera::Drag_sensitivity, (y_pos - last_cursor_pos.y) * Camera::Drag_sensitivity);
        Camera::Rotate(move.x, move.y);
        clip_update_needed = true;
    }
    last_cursor_pos = glm::vec2(x_pos, y_pos);
}

void WindowSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    Camera::UpdateProjectionMatrix(width, height);
    clip_update_needed = true;
}

static bool IsPressed(GLFWwindow *window, int key)
{
    bool pressed = glfwGetKey(window, key) == GLFW_PRESS;
    clip_update_needed = clip_update_needed || pressed;
    return pressed;
}

static void ProcessInput(GLFWwindow *window)
{
    if (IsPressed(window, GLFW_KEY_RIGHT))
        Camera::Rotate(int(Camera::Move::RIGHT), int(Camera::Move::STAY));

    if (IsPressed(window, GLFW_KEY_LEFT))
        Camera::Rotate(int(Camera::Move::LEFT), int(Camera::Move::STAY));

    if (IsPressed(window, GLFW_KEY_UP))
    {
        if (IsPressed(window, GLFW_KEY_LEFT_SHIFT) || IsPressed(window, GLFW_KEY_RIGHT_SHIFT))
        {
            Camera::Zoom(int(Camera::Move::IN));
            sphere.SetCameraDistanceU(Camera::Distance());
        }
        else
        {
            Camera::Rotate(int(Camera::Move::STAY), int(Camera::Move::UP));
        }
    }

    if (IsPressed(window, GLFW_KEY_DOWN))
    {
        if (IsPressed(window, GLFW_KEY_LEFT_SHIFT) || IsPressed(window, GLFW_KEY_RIGHT_SHIFT))
        {
            Camera::Zoom(int(Camera::Move::OUT));
            sphere.SetCameraDistanceU(Camera::Distance());
        }
        else
        {
            Camera::Rotate(int(Camera::Move::STAY), int(Camera::Move::DOWN));
        }
    }
}

static void TryUpdateClip()
{
    if (clip_update_needed)
    {
        sphere.SetClipMatrixU(Camera::ClipSpaceMatrix());
        sphere.SetCameraCoordsU(Camera::Position());
        clip_update_needed = false;
    }
}

static std::pair<bool, bool> DisplayRotationContent(Rotation &rotation, const std::string &id)
{
    std::pair<bool, bool> changed = {false, false};

    std::string angle_label = "Angle##" + id;
    std::string axis_label = "Axis Point##" + id;
    std::string color_label = "Color##" + id;

    ImGui::SameLine(); 
    if (ImGui::InputFloat(angle_label.c_str(), &rotation.Angle, 0.1f, 1.0f, "%.2f"))
    {
        rotation.Angle = std::max(rotation.Angle, -360.0f);
        rotation.Angle = std::min(rotation.Angle, 360.0f);
        changed.first = true;
    }

    ImGui::SameLine(); 
    if (ImGui::InputFloat3(axis_label.c_str(), glm::value_ptr(rotation.Axis), "%.2f")) 
        changed.first = true;

    ImGui::SameLine(); 
    if (ImGui::ColorEdit3(color_label.c_str(), glm::value_ptr(rotation.Color), ImGuiColorEditFlags_NoInputs)) 
        changed.second = true;
        
    // Пока убрал удаление, потому что ещё не придумал, как его правильно реализовать
    // ImGui::SameLine(); if (ImGui::SmallButton("Delete Rotation")) {} // Если у поворота есть активные потомки, то выкидывай окошко для подтверждения (с галочкой "больше не спрашивать")  

    return changed;
}

static void TryApplyChanges(const std::pair<bool, bool> &changes, unsigned int rotation_ind)
{
    if (!changes.first && !changes.second)
        return;

    sphere.UpdateRotation(rotation_ind, changes.first, changes.second);
}

static bool DisplayRotationNode(unsigned int ind)
{
    if (!sphere.RotationByIndex(ind).IsActive())
    {
        if (ImGui::Button("Add Rotation", {200.0f, 20.0f}))
            sphere.AddRotation(ind);
        return false;
    }

    std::string id = "##Node" + std::to_string(ind);
    if (sphere.Rotations()[ind].second != -1)
    {
        bool opened = ImGui::TreeNodeEx(id.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);
        TryApplyChanges(DisplayRotationContent(sphere.RotationByIndex(ind), std::to_string(ind)), ind);

        if (opened)
        {
            for (unsigned int i = 0; i < Rotation::Max_children; i++)
                if (!DisplayRotationNode(sphere.Rotations()[ind].second + i))
                    break;
            ImGui::TreePop();
        }
    }
    else
    {
        ImGui::Bullet();
        TryApplyChanges(DisplayRotationContent(sphere.RotationByIndex(ind), std::to_string(ind)), ind);
    }

    return true;
}

static void DrawUiWindow()
{
    // ImGui::ShowDemoWindow();
    ImGui::SetNextWindowSizeConstraints({400, -1}, {INFINITY, -1});
    if (!ImGui::Begin("##UiWindow", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("SPHERE PROPERTIES:");
    if(sphere.Detail_level && ImGui::SliderInt("Level of Detail", &(sphere.Detail_level), 1, sphere.MaxDetailLevel()))
        sphere.UpdateSphereShape();
    if (ImGui::ColorEdit3("Base Color", glm::value_ptr(sphere.Base_color)))
        sphere.UpdateSphereBaseColor();

    ImGui::Separator();
    ImGui::Text("ROTATIONS:");

    for (unsigned int i = 0; i < Rotation::Max_children; i++)
        if (!DisplayRotationNode(i))
            break;

    ImGui::End();
}

int main()
{
    glfwSetErrorCallback(ErrorCallback);

    if (!glfwInit())
    {
        std::cout << "LAUNCH ERROR: Glfw initialization failed" << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    unsigned int width = 1280;
    unsigned int height = 720;
    GLFWwindow *window = glfwCreateWindow(width, height, "coursework-2", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "LAUNCH ERROR: Window creation failed" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    int gl_version = gladLoadGL(glfwGetProcAddress);
    if (gl_version == 0)
    {
        std::cout << "LAUNCH ERROR: Glad initialization failed" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetWindowSizeCallback(window, WindowSizeCallback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    glfwSwapInterval(1);
    glViewport(0, 0, width, height);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Camera::UpdateProjectionMatrix(width, height);
    Camera::UpdatePosition();

    sphere = Sphere(30, {"../shaders/sphere.vert", "../shaders/sphere.frag"});
    sphere.SetCameraDistanceU(Camera::Distance());

    double last_time = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.65f, 0.65f, 0.65f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        DrawUiWindow();

        sphere.Draw();
        
        ProcessInput(window);
        glfwPollEvents();
        TryUpdateClip();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        double now = glfwGetTime();
        delta_time = now - last_time;
        last_time = now;
    }
    
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}