#pragma once
#include "Mesh.h"
#include "Component.h"
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class Model : public Component {
	private:
		std::vector<Texture> loadedTextures;
		std::vector<Mesh> meshes;
		std::string directory;
		bool gammaCorrection;

		void loadModel(std::string const& path);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);

	public:
		glm::vec2 tiling;

		Model(const std::string& path, bool gamma = false);
		void draw(Shader& shader);
		void drawGeometry();

		static unsigned int textureFromFile(const char* path, const std::string& directory, bool gamma = false);
};