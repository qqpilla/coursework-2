#include <iostream>

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "glm/vec2.hpp"

#include "sphere.hpp"
#include "camera.hpp"
#include "ui.hpp"
#include "dirs.hpp"

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

int main()
{
    setlocale(LC_ALL, "ru_RU.utf8");

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

    glfwSwapInterval(1);
    glViewport(0, 0, width, height);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Camera::UpdateProjectionMatrix(width, height);
    Camera::UpdatePosition();

    sphere = Sphere(30, {SHADERS_DIR "/sphere.vert", SHADERS_DIR "/sphere.frag"});
    sphere.SetCameraDistanceU(Camera::Distance());
    UI ui(&sphere, window, glsl_version);

    double last_time = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.65f, 0.65f, 0.65f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ui.BeginFrame();
        ui.DrawPropertiesWindow();
        ui.DrawRotationsResultsWindow();

        sphere.Draw();
        
        ProcessInput(window);
        glfwPollEvents();
        TryUpdateClip();
        ui.EndFrame();

        glfwSwapBuffers(window);

        double now = glfwGetTime();
        delta_time = now - last_time;
        last_time = now;
    }

    ui.Die();
    glfwTerminate();
}