#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>
#include <string>

class Skybox
{
public:
	int loadCubemap(std::vector<std::string> faces);

	unsigned int textureId;
};

#endif