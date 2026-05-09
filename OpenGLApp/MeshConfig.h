#pragma once
#include <glad/glad.h>

namespace MeshConfig {
	const unsigned int ALBEDO_SLOT = 4;
	const unsigned int NORMAL_SLOT = 5;
	const unsigned int METALLIC_SLOT = 6;
	const unsigned int ROUGHNESS_SLOT = 7;
	const unsigned int AO_SLOT = 8;

	const int MAX_BONE_INFLUENCE = 4;
	const unsigned int STARTING_TEXTURE_INDEX = 4; // start at texture 4 since 0-2 are used for IBL data and 3 for shadow map
	const unsigned int STARTING_TEXTURE_GL_INDEX = GL_TEXTURE0 + STARTING_TEXTURE_INDEX; 
}