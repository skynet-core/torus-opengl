//
// Created by skyne on 4/26/2022.
//

#include "ShaderProgram.h"
#include <iostream>
#include "Utils.h"

std::vector<ShaderFile> ShaderProgram::fill_shaders(const std::string_view &vertexShader,
                                                    const std::string_view &fragmentShader,
                                                    const std::string_view &geometryShader) {
    std::vector<ShaderFile> files{
            ShaderFile{Utils::resolve_path(vertexShader), (unsigned int) GL_VERTEX_SHADER},
            ShaderFile{Utils::resolve_path(fragmentShader), (unsigned int) GL_FRAGMENT_SHADER},
    };
    if (geometryShader != "") {
        files.emplace_back(
                ShaderFile{Utils::resolve_path(geometryShader), (unsigned int) GL_GEOMETRY_SHADER});
    }
    return files;
}


ShaderProgram::ShaderProgram(const std::vector<ShaderFile> &shaders) {
    std::vector<unsigned int> shader_ids{};
    auto clean_up = [&]() {
        for (auto &&id: shader_ids) {
            glDeleteShader(id);
        }
    };

    std::vector<char> buffer(1024);
    for (auto &&shaderFile: shaders) {
        auto code = Utils::read_file(Utils::resolve_path(shaderFile.get_path()));
        const char *code_c = code.c_str();

        auto id = glCreateShader(shaderFile.get_type());
        shader_ids.emplace_back(id);

        glShaderSource(id, 1, &code_c, nullptr);
        glCompileShader(id);

        if (check_shader_errors(id, &buffer[0], buffer.size())) {
            std::cerr << "ERROR: SHADER: " << std::string{&buffer[0]} << std::endl;
            clean_up();
            return;
        }
    }

    id = glCreateProgram();
    for (auto &&shader_id: shader_ids) {
        glAttachShader(id, shader_id);
    }
    glLinkProgram(id);
    clean_up();

    if (check_program_errors(id, &buffer[0], buffer.size())) {
        std::cerr << "ERROR: PROGRAM: " << std::string{&buffer[0]} << std::endl;
        glDeleteProgram(id);
        return;
    }

}

ShaderProgram::ShaderProgram(const std::string_view &vertexShader,
                             const std::string_view &fragmentShader,
                             const std::string_view &geometryShader) : ShaderProgram(
        ShaderProgram::fill_shaders(
                vertexShader,
                fragmentShader,
                geometryShader)) {
}


bool ShaderProgram::check_shader_errors(unsigned int shader, char *buffer, size_t size) {
    int success = 1;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, size, nullptr, buffer);
    }
    return success == 0;
}


bool ShaderProgram::check_program_errors(unsigned int program, char *buffer, size_t size) {
    int success = 1;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, size, nullptr, buffer);
    }
    return success == 0;
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(id);
}

unsigned int ShaderProgram::get_id() const {
    return id;
}

void ShaderProgram::activate() const {
    glUseProgram(id);
}


void setv(unsigned int id, const char *name, float value) {
    unsigned int loc = glGetUniformLocation(id, name);
    glUniform1f(loc, value);
}

void setv(unsigned int id, const char *name, unsigned int value) {
    unsigned int loc = glGetUniformLocation(id, name);
    glUniform1ui(loc, value);
}


void setv(unsigned int id, const char *name, int value) {
    unsigned int loc = glGetUniformLocation(id, name);
    glUniform1i(loc, value);
}

void setv(unsigned int id, const char *name, const glm::vec<2, float> &value) {
    unsigned int loc = glGetUniformLocation(id, name);
    glUniform2fv(loc, 1, &value[0]);
}

void setv(unsigned int id, const char *name, const glm::vec<3, float> &value) {
    unsigned int loc = glGetUniformLocation(id, name);
    glUniform3fv(loc, 1, &value[0]);
}

void setv(unsigned int id, const char *name, const glm::vec<4, float> &value) {
    unsigned int loc = glGetUniformLocation(id, name);
    glUniform4fv(loc, 1, &value[0]);
}

void setv(unsigned int id, const char *name, const glm::vec<2, unsigned int> &value) {
    unsigned int loc = glGetUniformLocation(id, name);
    glUniform2uiv(loc, 1, &value[0]);
}

void setv(unsigned int id, const char *name, const glm::vec<3, unsigned int> &value) {
    unsigned int loc = glGetUniformLocation(id, name);
    glUniform3uiv(loc, 1, &value[0]);
}

void setv(unsigned int id, const char *name, const glm::vec<4, unsigned int> &value) {
    unsigned int loc = glGetUniformLocation(id, name);
    glUniform4uiv(loc, 1, &value[0]);
}

void setv(unsigned int id, const char *name, const glm::vec<2, int> &value) {
    unsigned int loc = glGetUniformLocation(id, name);
    glUniform2iv(loc, 1, &value[0]);
}

void setv(unsigned int id, const char *name, const glm::vec<3, int> &value) {
    unsigned int loc = glGetUniformLocation(id, name);
    glUniform3iv(loc, 1, &value[0]);
}

void setv(unsigned int id, const char *name, const glm::vec<4, int> &value) {
    unsigned int loc = glGetUniformLocation(id, name);
    glUniform4iv(loc, 1, &value[0]);
}

extern void setv(unsigned int id, const char *name, const glm::mat<3, 3, float> &value) {
    unsigned int loc = glGetUniformLocation(id, name);
    glUniformMatrix3fv(loc, 1, GL_FALSE, &value[0][0]);
}

extern void setv(unsigned int id, const char *name, const glm::mat<4, 4, float> &value) {
    unsigned int loc = glGetUniformLocation(id, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]);
}
