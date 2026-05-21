#include "SkeletalModel.h"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

Shader* SkeletalModel::skeletalShader = nullptr;

SkeletalModel::SkeletalModel(const std::string& path, bool gamma) : gammaCorrection(gamma), boneCounter(0) {
	if (skeletalShader == nullptr) {
		skeletalShader = new Shader("shaders/anim_model.vs", "shaders/anim_model.fs");
	}

	loadModel(path);
}

void SkeletalModel::draw(Shader& shader) {
	size_t n = meshes.size();
	for (size_t i = 0; i < n; i++) {
		meshes.at(i).draw(shader);
	}
}

void SkeletalModel::drawGeometry() {
	size_t n = meshes.size();
	for (size_t i = 0; i < n; i++) {
		meshes.at(i).drawGeometry();
	}
}

std::map<std::string, BoneInfo>& SkeletalModel::GetBoneInfoMap() {
	return boneInfoMap; 
}

int& SkeletalModel::GetBoneCount() { 
	return boneCounter; 
}

void SkeletalModel::loadModel(const std::string& path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
}

void SkeletalModel::processNode(aiNode* node, const aiScene* scene) {

	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}

}

void SkeletalModel::setVertexBoneDataToDefault(Vertex& vertex) {
	for (int i = 0; i < MeshConfig::MAX_BONE_INFLUENCE; i++) {
		vertex.boneIds[i] = -1;
		vertex.weights[i] = 0.0f;
	}
}


Mesh SkeletalModel::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		setVertexBoneDataToDefault(vertex);
		vertex.position = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);
		vertex.normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);

		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;
		}
		else
			vertex.texCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	//1. diffuse maps
	std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	// 5. PBR
	// aiTextureType_BASE_COLOR
	std::vector<Texture> diffusePBR = loadMaterialTextures(material, aiTextureType_BASE_COLOR, "texture_PBR_diffuse");
	textures.insert(textures.end(), diffusePBR.begin(), diffusePBR.end());

	// aiTextureType_NORMAL_CAMERA
	std::vector<Texture> normalPBR = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_PBR_normal");
	textures.insert(textures.end(), normalPBR.begin(), normalPBR.end());

	// aiTextureType_METALNESS
	std::vector<Texture> metallicPBR = loadMaterialTextures(material, aiTextureType_METALNESS, "texture_PBR_metallic");
	textures.insert(textures.end(), metallicPBR.begin(), metallicPBR.end());

	// aiTextureType_DIFFUSE_ROUGHNESS
	std::vector<Texture> roughnessPBR = loadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_PBR_roughness");
	textures.insert(textures.end(), roughnessPBR.begin(), roughnessPBR.end());

	// aiTextureType_AMBIENT_OCCLUSION
	std::vector<Texture> aoPBR = loadMaterialTextures(material, aiTextureType_AMBIENT_OCCLUSION, "texture_PBR_ambient_occlusion");
	textures.insert(textures.end(), aoPBR.begin(), aoPBR.end());

	extractBoneWeightForVertices(vertices, mesh, scene);

	return Mesh(vertices, indices, textures);
}

void SkeletalModel::setVertexBoneData(Vertex& vertex, int boneID, float weight) {
	for (int i = 0; i < MeshConfig::MAX_BONE_INFLUENCE; ++i) {
		if (vertex.boneIds[i] < 0) {
			vertex.weights[i] = weight;
			vertex.boneIds[i] = boneID;
			break;
		}
	}
}


void SkeletalModel::extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene) {
	auto& nameToBoneInfo = boneInfoMap;
	int& boneCount = boneCounter;

	for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	{
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (nameToBoneInfo.find(boneName) == nameToBoneInfo.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = boneCount;
			newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
			nameToBoneInfo[boneName] = newBoneInfo;
			boneID = boneCount;
			boneCount++;
		}
		else
		{
			boneID = nameToBoneInfo[boneName].id;
		}
		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vertices.size());
			setVertexBoneData(vertices[vertexId], boneID, weight);
		}
	}
}


unsigned int SkeletalModel::textureFromFile(const char* path, const std::string& directory, bool gamma) {
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

std::vector<Texture> SkeletalModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; 
				break;
			}
		}
		if (!skip) {  
			Texture texture;
			texture.id = textureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture); 
		}
	}
	return textures;
}