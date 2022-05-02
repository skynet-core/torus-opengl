//
// Created by skyne on 4/26/2022.
//

#ifndef TORUS_SHADER_PROG_H
#define TORUS_SHADER_PROG_H

#include "ShaderFile.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string_view>
#include <type_traits>
#include <iostream>

extern void setv(unsigned int id, const char *name, float value);

extern void setv(unsigned int id, const char *name, unsigned int value);

extern void setv(unsigned int id, const char *name, int value);

extern void setv(unsigned int id, const char *name, const glm::vec<2, float> &value);

extern void setv(unsigned int id, const char *name, const glm::vec<3, float> &value);

extern void setv(unsigned int id, const char *name, const glm::vec<4, float> &value);

extern void setv(unsigned int id, const char *name, const glm::vec<2, unsigned int> &value);

extern void setv(unsigned int id, const char *name, const glm::vec<3, unsigned int> &value);

extern void setv(unsigned int id, const char *name, const glm::vec<4, unsigned int> &value);

extern void setv(unsigned int id, const char *name, const glm::vec<2, int> &value);

extern void setv(unsigned int id, const char *name, const glm::vec<3, int> &value);

extern void setv(unsigned int id, const char *name, const glm::vec<4, int> &value);

extern void setv(unsigned int id, const char *name, const glm::mat<3, 3, float> &value);

extern void setv(unsigned int id, const char *name, const glm::mat<4, 4, float> &value);


class ShaderProgram {
public:
    explicit ShaderProgram(const std::string_view &vertexShader,
                           const std::string_view &fragmentShader,
                           const std::string_view &geometryShader = "");

    explicit ShaderProgram(const std::vector<ShaderFile> &files);

    ~ShaderProgram();

    [[nodiscard]] unsigned int get_id() const;

    void activate() const;

    template<typename T>
    void set(const char *name, const T &value) const;

private:
    static std::vector<ShaderFile> fill_shaders(const std::string_view &vertexShader,
                                                const std::string_view &fragmentShader,
                                                const std::string_view &geometryShader);

    static bool check_shader_errors(unsigned int shader, char *buffer, size_t size);

    static bool check_program_errors(unsigned int shader, char *buffer, size_t size);

    unsigned int id;
};

template<typename T>
void ShaderProgram::set(const char *name, const T &value) const {
    setv(id, name, value);
}


#endif // TORUS_SHADER_H
