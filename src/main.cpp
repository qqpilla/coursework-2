#include <iostream>

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/vec2.hpp"

#include "icosphere.hpp"
#include "shader_program.hpp"
#include "camera.hpp"

static bool clip_update_needed = true;
static glm::vec2 last_cursor_pos = glm::vec2(0.0, 0.0);

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
    clip_update_needed = true;
}

void CursorPosCallback(GLFWwindow *window, double x_pos, double y_pos)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        glm::vec2 move(-(x_pos - last_cursor_pos.x), y_pos - last_cursor_pos.y);
        Camera::Rotate(move.x, move.y);
        clip_update_needed = true;
    }
    last_cursor_pos = glm::vec2(x_pos, y_pos);
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
            Camera::Zoom(int(Camera::Move::IN));
        else
            Camera::Rotate(int(Camera::Move::STAY), int(Camera::Move::UP));
    }

    if (IsPressed(window, GLFW_KEY_DOWN))
    {
        if (IsPressed(window, GLFW_KEY_LEFT_SHIFT) || IsPressed(window, GLFW_KEY_RIGHT_SHIFT))
            Camera::Zoom(int(Camera::Move::OUT));
        else
            Camera::Rotate(int(Camera::Move::STAY), int(Camera::Move::DOWN));
    }
}

static void TryUpdateClip(unsigned int u_clip_matrix_loc)
{
    if (clip_update_needed)
    {
        glUniformMatrix4fv(u_clip_matrix_loc, 1, GL_FALSE, glm::value_ptr(Camera::ClipSpaceMatrix()));
        clip_update_needed = false;
    }
}

static void SetWireframeMode(unsigned int u_wireframe_mode_loc, bool value)
{
    glUniform1i(u_wireframe_mode_loc, value);
    glPolygonMode(GL_FRONT, value ? GL_LINE : GL_FILL);
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

    glfwSwapInterval(1);
    glViewport(0, 0, width, height);
    glEnable(GL_CULL_FACE);
    Camera::UpdateProjectionMatrix(width, height);

    Icosphere::GenerateIcosphere(2);
    Icosphere::SetUpRendering(glm::vec3(0.2f, 0.2f, 0.2f));

    ShaderProgram icosphere_shader("../shaders/icosphere.vert", "../shaders/icosphere.frag");
    unsigned int u_clip_matrix_loc = glGetUniformLocation(icosphere_shader.ID(), "u_clip_matrix");
    unsigned int u_wireframe_mode_loc = glGetUniformLocation(icosphere_shader.ID(), "u_wireframe_mode");
    unsigned int u_wireframe_color_loc = glGetUniformLocation(icosphere_shader.ID(), "u_wireframe_color");

    glUseProgram(icosphere_shader.ID());
    glUniform3fv(u_wireframe_color_loc, 1, glm::value_ptr(Icosphere::wireframe_color));

    double last_time = 0.0f;
    double timer = 0.0f;
    int fps = 0;
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.65f, 0.65f, 0.65f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        SetWireframeMode(u_wireframe_mode_loc, false);
        Icosphere::Render();
        SetWireframeMode(u_wireframe_mode_loc, true);
        Icosphere::Render();
        
        ProcessInput(window);
        glfwPollEvents();
        TryUpdateClip(u_clip_matrix_loc);

        glfwSwapBuffers(window);

        double now = glfwGetTime();
        delta_time = now - last_time;
        last_time = now;
    }
    
    glfwTerminate();
}