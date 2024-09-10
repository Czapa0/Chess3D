#ifndef BEZIER_H
#define BEZIER_H

#include <vector>

#include <glm/glm.hpp>

#include "Shader.h"

class Bezier
{
public:
	Bezier() = default;
	Bezier(std::vector<glm::vec3> corners, glm::vec3 color);
	void Update(float dt);
	void Draw(Shader& shader);

	unsigned int VBO, VAO, vertexCount;
	std::vector<glm::vec3> controlPoints;
	std::vector<glm::vec3> controlPointVelocities;
	glm::vec3 color;
	glm::mat4 model;

private:
	glm::vec3 TensionFromNeighbour(int x, int y, glm::vec3 position);
};

#endif