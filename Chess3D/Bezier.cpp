#include "Bezier.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

Bezier::Bezier(std::vector<glm::vec3> corners, glm::vec3 color) : color(color), vertexCount(16), model(1.0f) {

	controlPoints.reserve(vertexCount);
	controlPointVelocities.reserve(vertexCount);

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

	// model matrix
	model = glm::scale(model, glm::vec3(3.0f));
	model = glm::translate(model, glm::vec3(0.0f, -0.01f, 0.0f));
	model = glm::rotate(model, glm::radians(-135.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	// init VAO and VBO
	glCreateBuffers(1, &VBO);
	glCreateVertexArrays(1, &VAO);
	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 3 * sizeof(float));
	glNamedBufferStorage(
		VBO, vertexCount * 4 * 3, NULL, GL_DYNAMIC_STORAGE_BIT
	);
	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(VAO, 0, 0);
}

glm::vec3 Bezier::TensionFromNeighbour(int x, int y, glm::vec3 position) {

	glm::vec3 otherPosition = controlPoints[x + 4 * y];

	return otherPosition - position;
}

void Bezier::Update(float dt) {

	//For internal points:

	for (int x = 1; x < 3; ++x) {
		for (int y = 1; y < 3; ++y) {

			glm::vec3 net_force = glm::vec3(0.0f, 0.0f, -0.0025f);

			glm::vec3 position = controlPoints[x + 4 * y];

			glm::vec3 tension = TensionFromNeighbour(x - 1, y - 1, position);
			tension += TensionFromNeighbour(x, y - 1, position);
			tension += TensionFromNeighbour(x + 1, y - 1, position);
			tension += TensionFromNeighbour(x - 1, y, position);
			tension += TensionFromNeighbour(x + 1, y, position);
			tension += TensionFromNeighbour(x - 1, y + 1, position);
			tension += TensionFromNeighbour(x, y + 1, position);
			tension += TensionFromNeighbour(x + 1, y + 1, position);
			net_force += 0.0002f * tension;

			controlPointVelocities[x + 4 * y] += dt * net_force;
		}
	}


	for (int x = 1; x < 3; ++x) {
		for (int y = 1; y < 3; ++y) {

			controlPoints[x + 4 * y] += dt * controlPointVelocities[x + 4 * y];

		}
	}

}

void Bezier::Draw(Shader& shader)
{
	std::vector<float> vertices;
	vertices.reserve(3 * vertexCount);
	for (glm::vec3 vertex : controlPoints) {
		vertices.push_back(vertex.x);
		vertices.push_back(vertex.y);
		vertices.push_back(vertex.z);
	}
	glPatchParameteri(GL_PATCH_VERTICES, vertexCount);
	glNamedBufferSubData(VBO, 0, vertices.size() * sizeof(float), vertices.data());
	glBindVertexArray(VAO);
	glDrawArrays(GL_PATCHES, 0, vertexCount);
	glBindVertexArray(0);
}
