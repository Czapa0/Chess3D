#include "Light.h"
#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Light::Light(glm::vec3 ambient,
    glm::vec3 diffuse,
    glm::vec3 specular,
    unsigned int id) :
    ambient(ambient),
    diffuse(diffuse),
    specular(specular),
    id(id) { }

unsigned int PointLight::pointLightCount = 0;

PointLight::PointLight(glm::vec3 ambient,
    glm::vec3 diffuse,
    glm::vec3 specular,
    glm::vec3 position,
    float constant,
    float linear,
    float quadratic) :
    Light(ambient, diffuse, specular, pointLightCount++),
    position(position), constant(constant), linear(linear), quadratic(quadratic) {
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, NEAR_PLANE_PL, FAR_PLANE_PL);
    shadowTransformations.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransformations.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransformations.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    shadowTransformations.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    shadowTransformations.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransformations.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
}

unsigned int SpotLight::spotlightCount = 0;

SpotLight::SpotLight(glm::vec3 ambient,
    glm::vec3 diffuse,
    glm::vec3 specular,
    glm::vec3 direction,
    glm::vec3 position,
    float constant,
    float linear,
    float quadratic,
    float cone) :
    Light(ambient, diffuse, specular, spotlightCount++),
    position(position), constant(constant), linear(linear), quadratic(quadratic), direction(direction), initialDirection(direction), cone(cone) {
    shadowProjMat = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, NEAR_PLANE_PL, FAR_PLANE_PL);
    updateShadowTransformation();
}

void SpotLight::updateShadowTransformation()
{
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::cross(direction, worldUp);
    glm::vec3 up = glm::cross(right, direction);
    shadowTransformation = shadowProjMat * glm::lookAt(position, position + direction, glm::normalize(up));
}

DirectionalLight::DirectionalLight(glm::vec3 ambient,
    glm::vec3 diffuse,
    glm::vec3 specular,
    glm::vec3 direction) :
    Light(ambient, diffuse, specular, 0), direction(direction), initialDirection(direction) {
    shadowProjMat = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, NEAR_PLANE_PL, FAR_PLANE_PL);
    updateShadowTransformation();
}

void DirectionalLight::updateShadowTransformation()
{
    shadowTransformation = shadowProjMat * glm::lookAt(-direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}