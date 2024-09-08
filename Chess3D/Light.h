#ifndef LIGHT_H
#define LIGHT_H

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <vector>

constexpr unsigned int SHADOW_WIDTH = 1024;
constexpr unsigned int SHADOW_HEIGHT = 1024;

constexpr float NEAR_PLANE_PL = 0.1f;
constexpr float FAR_PLANE_PL = 15.0f;

struct Light {
    unsigned int id;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    Light() = default;
    Light(glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular,
        unsigned int id);
};

struct PointLight : Light {
    static unsigned int pointLightCount;

    glm::vec3 position;

    float constant;
    float linear;
    float quadratic;

    std::vector<glm::mat4> shadowTransformations;

    PointLight() = default;
    PointLight(glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular,
        glm::vec3 position,
        float constant,
        float linear,
        float quadratic);
};

struct SpotLight : Light {
    static unsigned int spotlightCount;

    glm::vec3 position;

    float constant;
    float linear;
    float quadratic;

    glm::mat4 shadowProjMat;
    glm::mat4 shadowTransformation;

    glm::vec3 direction;
    glm::vec3 initialDirection;
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
    void updateShadowTransformation();
};

struct DirectionalLight : Light {
    glm::mat4 shadowProjMat;
    glm::mat4 shadowTransformation;

    glm::vec3 direction;
    glm::vec3 initialDirection;

    DirectionalLight() = default;
    DirectionalLight(glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular,
        glm::vec3 direction);
    void updateShadowTransformation();
};

#endif