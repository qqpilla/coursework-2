#include <iostream>

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/type_ptr.hpp"

#include "icosphere.hpp"
#include "shader_program.hpp"
#include "camera.hpp"

static unsigned int clip_matrix_loc;

void ErrorCallback(int error_code, const char *message)
{
    std::cout << "ERROR: " << message << "\nERROR CODE: " << error_code << std::endl;
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
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

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    Camera::UpdateProjectionMatrix(width, height);

    Icosphere::GenerateIcosphere();
    Icosphere::SetUpRendering(glm::vec3(0.2f, 0.2f, 0.2f));

    ShaderProgram icosphere_shader("../shaders/icosphere.vert", "../shaders/icosphere.frag");
    clip_matrix_loc = glGetUniformLocation(icosphere_shader.ID(), "u_clip_matrix");

    glUseProgram(icosphere_shader.ID());

    glUniformMatrix4fv(clip_matrix_loc, 1, GL_FALSE, glm::value_ptr(Camera::ClipSpaceMatrix()));

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.65f, 0.65f, 0.65f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Icosphere::Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
}