#pragma once

#include "MeshConfig.h"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <shader.h>

#include <string>
#include <vector>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	glm::vec3 tangent;
	glm::vec3 bitangent;
	int boneIds[MeshConfig::MAX_BONE_INFLUENCE];
	float weights[MeshConfig::MAX_BONE_INFLUENCE];
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh {
	private:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		unsigned int vao;
		unsigned int vbo;
		unsigned int ebo;

		static unsigned int defaultAlbedo;
		static unsigned int defaultNormal;
		static unsigned int defaultMetallic;
		static unsigned int defaultRoughness;
		static unsigned int defaultAO;

		void setupMesh();
		static unsigned int createDefaultTexture(unsigned char r, unsigned char g, unsigned char b);

	public:
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
		void draw(Shader& shader);
		void drawGeometry();
};