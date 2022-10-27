#pragma once

#include "Define.h"
#include <string>
#include <iostream>
#include <fstream>
#include "OpenGLDefines.h"

// cpp로 분할하기

static string ReadShader(const char* filename)
{
    string shaderCode;
    ifstream file(filename, std::ios::in);

    if (!file.good())
    {
        MSGBox("Open Shadeer File Failed");
        terminate();
    }

    file.seekg(0, ios::end);
    shaderCode.resize((unsigned int)file.tellg());
    file.seekg(0, ios::beg);
    file.read(&shaderCode[0], shaderCode.size());
    file.close();

    return shaderCode;
}

static GLuint CreateShader(GLenum shaderType, string source)    
{
    int compile_result = 0;

    GLuint shader = glCreateShader(shaderType);
    const char* shader_code_ptr = source.c_str();
    const int shader_code_size = source.size();

    glShaderSource(shader, 1, &shader_code_ptr, &shader_code_size);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_result);

    if (compile_result == GL_FALSE)
        return 0;

    return shader;
}

static GLuint CreateShaderProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename)
{
    string vertex_shader_code = ReadShader(vertexShaderFilename);
    string fragment_shader_code = ReadShader(fragmentShaderFilename);

    GLuint vertex_shader = CreateShader(GL_VERTEX_SHADER, vertex_shader_code);
    GLuint fragment_shader = CreateShader(GL_FRAGMENT_SHADER, fragment_shader_code);

    int link_result = 0;

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &link_result);


    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &link_result);

   

    if (link_result == GL_FALSE)
        return 0;

    glUseProgram(program);

    return program;
}