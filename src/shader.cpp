#include "shader.hpp"
#include "glad/glad.h"
#include <fstream>
#include <sstream>
#include <iostream>

#include "utils/shaderInclude.hpp"

std::string get_file_contents(const char* filename) {
    std::ifstream in(filename, std::ios::binary);
    if (in) {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    throw(errno);
}

nell::Shader::Shader(const char *v_path, const char *f_path) {
    // get glsl source
    std::string v_code_string = Shadinclude::load(v_path);
    std::string f_code_string = Shadinclude::load(f_path);

    const char *v_shader_source = v_code_string.c_str();
    const char *f_shader_source = f_code_string.c_str();

    // create shaders
    unsigned int vertex, fragment;

    int success;
    char info_log[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &v_shader_source, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, info_log);
        std::cout << "[error]shader: vertex shader compilation failed\n" << info_log << std::endl;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &f_shader_source, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, info_log);
        std::cout << "[error]shader: fragment shader compilation failed\n" << info_log << std::endl;
    }

    // 着色器程序
    this->id = glCreateProgram();
    glAttachShader(this->id, vertex);
    glAttachShader(this->id, fragment);
    glLinkProgram(this->id);

    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(this->id, 512, NULL, info_log);
        std::cout << "[error]shader: program linking failed\n" << info_log << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void nell::Shader::use() {
    glUseProgram(this->id);
}

void nell::Shader::set_bool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(this->id, name.c_str()), (int)value);
}

void nell::Shader::set_int(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(this->id, name.c_str()), value);
}
void nell::Shader::set_float(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(this->id, name.c_str()), value);
}

void nell::Shader::set_vec4(const std::string &name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(this->id, name.c_str()), x, y, z, w);
}