#ifndef BEZIER_H
#define BEZIER_H

#include <vector>

#include <glm/glm.hpp>

class Bezier
{
public:
	Bezier(std::vector<glm::vec3> corners, glm::vec3 color);

	std::vector<glm::vec3> controlPoints;
	std::vector<glm::vec3> controlPointVelocities;
	glm::vec3 color;
};

#endif