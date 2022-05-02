//
// Created by skyne on 4/25/2022.
//

#ifndef TORUS_TORUS_H
#define TORUS_TORUS_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Torus {
public:
    explicit Torus(float in_radius, float out_radius, int circles, int faces);

    explicit Torus(float in_radius, float out_radius, int circles);

    [[nodiscard]] std::vector<unsigned int> get_indices() const;

    [[nodiscard]] std::vector<glm::vec3> get_vertices() const;

    [[nodiscard]] std::vector<glm::vec3> get_normals() const;

    [[nodiscard]] std::vector<glm::vec2> get_text_coords() const;

private:
    float in_radius;
    float out_radius;
    int circles;
    int faces;

    std::vector<unsigned int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> tex_coords;
};


#endif //TORUS_TORUS_H
