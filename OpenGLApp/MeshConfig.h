#pragma once
#include <glad/glad.h>

namespace MeshConfig {
	const int MAX_BONE_INFLUENCE = 4;
	unsigned int STARTING_TEXTURE_INDEX = 4; // start at texture 4 since 0-2 are used for IBL data and 3 for shadow map
	unsigned int STARTING_TEXTURE_GL_INDEX = GL_TEXTURE0 + STARTING_TEXTURE_INDEX; 
}