#include "Bezier.h"

Bezier::Bezier(std::vector<glm::vec3> corners, glm::vec3 color) : color(color) {

	controlPoints.reserve(16);
	controlPointVelocities.reserve(16);

	glm::vec3 top_left = corners[0];
	glm::vec3 bottom_right = corners[1];
	float length = bottom_right.x - top_left.x;
	float width = bottom_right.y - top_left.y;

	for (float y = 0; y < 4; ++y) {
		for (float x = 0; x < 4; ++x) {
			controlPoints.push_back(
				glm::vec3(
					top_left.x + length * x / 4.0f,
					top_left.y + width * y / 4.0f,
					top_left.y + width * y / 4.0f
				)
			);
			controlPointVelocities.push_back(
				glm::vec3(0.0f, 0.0f, 0.0f)
			);
		}
	}
}