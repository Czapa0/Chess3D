#include "Light.h"
#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Light::Light(glm::vec3 ambient,
    glm::vec3 diffuse,
    glm::vec3 specular) :
    ambient(ambient),
    diffuse(diffuse),
    specular(specular) { }

unsigned int PointLight::pointLightCount = 0;

PointLight::PointLight(glm::vec3 ambient,
    glm::vec3 diffuse,
    glm::vec3 specular,
    glm::vec3 position,
    float constant,
    float linear,
    float quadratic) :
    Light(ambient, diffuse, specular),
    id(pointLightCount++), position(position), constant(constant), linear(linear), quadratic(quadratic) {
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, NEAR_PLANE_PL, FAR_PLANE_PL);
    shadowTransformataions[0] = shadowProj * glm::lookAt(position, position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    shadowTransformataions[1] = shadowProj * glm::lookAt(position, position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    shadowTransformataions[2] = shadowProj * glm::lookAt(position, position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    shadowTransformataions[3] = shadowProj * glm::lookAt(position, position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    shadowTransformataions[4] = shadowProj * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    shadowTransformataions[5] = shadowProj * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
}

SpotLight::SpotLight(glm::vec3 ambient,
    glm::vec3 diffuse,
    glm::vec3 specular,
    glm::vec3 direction,
    glm::vec3 position,
    float constant,
    float linear,
    float quadratic,
    float cone) :
    PointLight(ambient, diffuse, specular, position, constant, linear, quadratic),
    direction(direction), cone(cone) { }