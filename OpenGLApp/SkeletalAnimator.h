#pragma once

#include "Component.h"

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "bone.h"
#include "SkeletalAnimation.h"

class SkeletalAnimator : public Component {
	public:
		std::vector<glm::mat4> m_FinalBoneMatrices;
		SkeletalAnimation* m_CurrentAnimation;
		SkeletalAnimation* m_CurrentAnimation2;
		float m_CurrentTime;
		float m_CurrentTime2;
		float m_DeltaTime;
		float m_blendAmount;

		std::map<std::string, glm::mat4> m_BoneGlobalTransform;

		SkeletalAnimator(SkeletalAnimation* animation);

		virtual void update() override;

		void playAnimation(SkeletalAnimation* pAnimation, SkeletalAnimation* pAnimation2, float time1, float time2, float blend);
		glm::mat4 updateBlend(Bone* Bone1, Bone* Bone2);
		void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
		std::vector<glm::mat4> getFinalBoneMatrices();
		std::vector<std::string> getAllNodeNames();

	private:
		void getAllNodesNamesHelper(const AssimpNodeData* node, std::vector<std::string>& names);
};