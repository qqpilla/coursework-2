#include <iostream>

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/type_ptr.hpp"

#include "icosphere.hpp"
#include "shader_program.hpp"
#include "camera.hpp"

static bool clip_update_needed = true;
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
        
    Camera::Zoom(y_offset > 0 ? Camera::Move::IN : Camera::Move::OUT);
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
        Camera::Rotate(Camera::Move::RIGHT, Camera::Move::STAY);

    if (IsPressed(window, GLFW_KEY_LEFT))
        Camera::Rotate(Camera::Move::LEFT, Camera::Move::STAY);

    if (IsPressed(window, GLFW_KEY_UP))
    {
        if (IsPressed(window, GLFW_KEY_LEFT_SHIFT) || IsPressed(window, GLFW_KEY_RIGHT_SHIFT))
            Camera::Zoom(Camera::Move::IN);
        else
            Camera::Rotate(Camera::Move::STAY, Camera::Move::UP);
    }

    if (IsPressed(window, GLFW_KEY_DOWN))
    {
        if (IsPressed(window, GLFW_KEY_LEFT_SHIFT) || IsPressed(window, GLFW_KEY_RIGHT_SHIFT))
            Camera::Zoom(Camera::Move::OUT);
        else
            Camera::Rotate(Camera::Move::STAY, Camera::Move::DOWN);
    }
}

static void TryUpdateClip(unsigned int clip_matrix_loc)
{
    if (clip_update_needed)
    {
        glUniformMatrix4fv(clip_matrix_loc, 1, GL_FALSE, glm::value_ptr(Camera::ClipSpaceMatrix()));
        clip_update_needed = false;
    }
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

    glfwSwapInterval(1);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    Camera::UpdateProjectionMatrix(width, height);

    Icosphere::GenerateIcosphere(1);
    Icosphere::SetUpRendering(glm::vec3(0.2f, 0.2f, 0.2f));

    ShaderProgram icosphere_shader("../shaders/icosphere.vert", "../shaders/icosphere.frag");
    unsigned int clip_matrix_loc = glGetUniformLocation(icosphere_shader.ID(), "u_clip_matrix");

    glUseProgram(icosphere_shader.ID());

    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_LINE);

    double last_time = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.65f, 0.65f, 0.65f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Icosphere::Render();
        
        ProcessInput(window);
        glfwPollEvents();
        TryUpdateClip(clip_matrix_loc);

        glfwSwapBuffers(window);

        double now = glfwGetTime();
        delta_time = now - last_time;
        last_time = now;
    }
    
    glfwTerminate();
}