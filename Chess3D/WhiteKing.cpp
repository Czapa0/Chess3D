#include "WhiteKing.h"

WhiteKing::WhiteKing(std::string const& path, bool gamma) : Model(path, gamma)
{
	// init model matrix
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(10.7f, -0.6f, 0.0f));
}