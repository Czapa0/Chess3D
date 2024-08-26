#include "Chessboard.h"

Chessboard::Chessboard(std::string const& path, bool gamma) : Model(path, gamma)
{
	// init model matrix
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
}