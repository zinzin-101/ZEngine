#include "SkeletalAnimation.h"
#include "SkeletalModel.h"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp_glm_helpers.h>

SkeletalAnimation::SkeletalAnimation(const std::string& animationPath, SkeletalModel* model) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
	assert(scene && scene->mRootNode);
	auto animation = scene->mAnimations[0];
	duration = (float)animation->mDuration;
	ticksPerSecond = (float)animation->mTicksPerSecond;
	aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
	globalTransformation = globalTransformation.Inverse();
	readHierarchyData(rootNode, scene->mRootNode);
	readMissingBones(animation, *model);
}

SkeletalAnimation::~SkeletalAnimation() {}

Bone* SkeletalAnimation::findBone(const std::string& name) {
	auto iter = std::find_if(bones.begin(), bones.end(),
		[&](const Bone& Bone)
		{
			return Bone.GetBoneName() == name;
		}
	);
	if (iter == bones.end()) return nullptr;
	else return &(*iter);
}


float SkeletalAnimation::getTicksPerSecond() { 
	return (float)ticksPerSecond; 
}

float SkeletalAnimation::getDuration() { 
	return duration; 
}

const AssimpNodeData& SkeletalAnimation::getRootNode() { 
	return rootNode; 
}

const std::map<std::string, BoneInfo>& SkeletalAnimation::getBoneIDMap() {
	return boneInfoMap;
}

void SkeletalAnimation::readMissingBones(const aiAnimation* animation, SkeletalModel& model)
{
	int size = animation->mNumChannels;

	auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
	int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

	//reading channels(bones engaged in an animation and their keyframes)
	for (int i = 0; i < size; i++)
	{
		auto channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			boneInfoMap[boneName].id = boneCount;
			boneCount++;
		}
		bones.push_back(Bone(channel->mNodeName.data,
			boneInfoMap[channel->mNodeName.data].id, channel));
	}

	this->boneInfoMap = boneInfoMap;
}

void SkeletalAnimation::readHierarchyData(AssimpNodeData& dest, const aiNode* src)
{
	assert(src);

	dest.name = src->mName.data;
	dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
	dest.childrenCount = src->mNumChildren;

	for (unsigned int i = 0; i < src->mNumChildren; i++)
	{
		AssimpNodeData newData;
		readHierarchyData(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}