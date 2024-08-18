#include "Light.h"

Light::Light(glm::vec3 ambient,
    glm::vec3 diffuse,
    glm::vec3 specular) :
    ambient(ambient),
    diffuse(diffuse),
    specular(specular) { }

PointLight::PointLight(glm::vec3 ambient,
    glm::vec3 diffuse,
    glm::vec3 specular,
    glm::vec3 position,
    float constant,
    float linear,
    float quadratic) :
    Light(ambient, diffuse, specular),
    position(position), constant(constant), linear(linear), quadratic(quadratic) { }

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