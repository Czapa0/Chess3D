#ifndef LIGHT_H
#define LIGHT_H

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <array>

constexpr unsigned int SHADOW_WIDTH = 1024;
constexpr unsigned int SHADOW_HEIGHT = 1024;

constexpr float NEAR_PLANE_PL = 1.0f;
constexpr float FAR_PLANE_PL = 35.0f;

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
    static unsigned int pointLightCount;
    unsigned int id;

    glm::vec3 position;

    float constant;
    float linear;
    float quadratic;

    std::array<glm::mat4, 6> shadowTransformataions;

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