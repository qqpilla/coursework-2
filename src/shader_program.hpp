#pragma once

#include <string>
#include <unordered_map>

#include "glad/gl.h"

class ShaderProgram
{
private:
    unsigned int _program;
    std::unordered_map<std::string, unsigned int> _uniforms_locations;

    void CreateShaderProgram(const char *vertex_source, const char *fragment_source);
    unsigned int CompileShader(GLuint type, const char *source);

    void TryGetNewLocation(const GLchar *name);

public:
    ShaderProgram() {}
    ShaderProgram(std::string vertex_shader_path, std::string fragment_shader_path);

    unsigned int ID() const { return _program; }

    void SetUniform1i(const GLchar *name, GLint value);
    void SetUniform1f(const GLchar *name, GLfloat value);
    void SetUniform3fv(const GLchar *name, const GLfloat *value);
    void SetUniformMatrix4fv(const GLchar *name, const GLfloat *value);
};