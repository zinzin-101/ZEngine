#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <Bone.h>
#include <functional>
#include <animdata.h>
#include <string>

struct AssimpNodeData
{
	glm::mat4 transformation = glm::mat4(1.0f);
	std::string name;
	int childrenCount = 0;
	std::vector<AssimpNodeData> children;
};

class SkeletalModel;

class SkeletalAnimation {
	public:
		SkeletalAnimation() = default;
		SkeletalAnimation(const std::string& animationPath, SkeletalModel* model);
		~SkeletalAnimation();
		Bone* findBone(const std::string& name);

		float getTicksPerSecond();
		float getDuration();
		const AssimpNodeData& getRootNode();
		const std::map<std::string, BoneInfo>& getBoneIDMap();

	private:
		float duration;
		int ticksPerSecond;
		std::vector<Bone> bones;
		AssimpNodeData rootNode;
		std::map<std::string, BoneInfo> boneInfoMap;

		void readMissingBones(const aiAnimation* animation, SkeletalModel& model);
		void readHierarchyData(AssimpNodeData& dest, const aiNode* src);
};

