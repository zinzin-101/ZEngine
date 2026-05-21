#pragma once
#include "Mesh.h"
#include "Component.h"
#include <shader.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp_glm_helpers.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <animdata.h>

class SkeletalModel : public Component {
	public:
		static Shader* skeletalShader;

		std::vector<Texture> textures_loaded;
		std::vector<Mesh>    meshes;
		std::string directory;
		bool gammaCorrection;

		SkeletalModel(const std::string& path, bool gamma = false);

		void draw(Shader& shader);
		void drawGeometry();

		std::map<std::string, BoneInfo>& GetBoneInfoMap();
		int& GetBoneCount();

	private:

		std::map<std::string, BoneInfo> boneInfoMap;
		int boneCounter;

		void loadModel(const std::string& path);
		void processNode(aiNode* node, const aiScene* scene);
		void setVertexBoneDataToDefault(Vertex& vertex);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		void setVertexBoneData(Vertex& vertex, int boneID, float weight);
		void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
		unsigned int textureFromFile(const char* path, const std::string& directory, bool gamma = false);
		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};