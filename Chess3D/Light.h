#ifndef LIGHT_H
#define LIGHT_H

#include <glm/vec3.hpp>

struct Light {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    Light() = default;
    Light(glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular);
};

struct PointLight : Light {
    glm::vec3 position;

    float constant;
    float linear;
    float quadratic;

    PointLight() = default;
    PointLight(glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular,
        glm::vec3 position,
        float constant,
        float linear,
        float quadratic);
};

struct SpotLight : PointLight {
    glm::vec3 direction;
    float cone;

    SpotLight() = default;
    SpotLight(glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular,
        glm::vec3 direction,
        glm::vec3 position,
        float constant,
        float linear,
        float quadratic,
        float cone);
};
#endif