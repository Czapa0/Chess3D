#include "Material.h"

Material::Material(glm::vec3 ambient,
	glm::vec3 diffuse,
	glm::vec3 specular,
	float shininess) :
	ambient(ambient),
	diffuse(diffuse),
	specular(specular),
	shininess(shininess) { };