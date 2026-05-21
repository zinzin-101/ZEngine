#include "SkeletalAnimator.h"
#include "Engine.h"

SkeletalAnimator::SkeletalAnimator(SkeletalAnimation* animation) {
	m_CurrentTime = 0.0;
	m_CurrentAnimation = animation;
	m_CurrentAnimation2 = NULL;
	m_blendAmount = 0;

	m_FinalBoneMatrices.reserve(100);

	for (int i = 0; i < 100; i++)
		m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
}

void SkeletalAnimator::update() {
	float dt = Engine::getInstance()->getTime()->getDeltaTime();
	m_DeltaTime = dt;
	if (m_CurrentAnimation)
	{
		m_CurrentTime += m_CurrentAnimation->getTicksPerSecond() * dt;
		m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->getDuration());

		if (m_CurrentAnimation2)
		{
			m_CurrentTime2 += m_CurrentAnimation2->getTicksPerSecond() * dt;
			m_CurrentTime2 = fmod(m_CurrentTime2, m_CurrentAnimation2->getDuration());
		}

		calculateBoneTransform(&m_CurrentAnimation->getRootNode(), glm::mat4(1.0f));
	}
}

void SkeletalAnimator::playAnimation(SkeletalAnimation* pAnimation, SkeletalAnimation* pAnimation2, float time1, float time2, float blend)
{
	m_CurrentAnimation = pAnimation;
	m_CurrentTime = time1;
	m_CurrentAnimation2 = pAnimation2;
	m_CurrentTime2 = time2;
	m_blendAmount = blend;
}

glm::mat4 SkeletalAnimator::updateBlend(Bone* Bone1, Bone* Bone2) {
	glm::vec3 bonePos1, bonePos2, finalPos;
	glm::vec3 boneScale1, boneScale2, finalScale;
	glm::quat boneRot1, boneRot2, finalRot;

	Bone1->InterpolatePosition(m_CurrentTime, bonePos1);
	Bone2->InterpolatePosition(m_CurrentTime2, bonePos2);
	Bone1->InterpolateRotation(m_CurrentTime, boneRot1);
	Bone2->InterpolateRotation(m_CurrentTime2, boneRot2);
	Bone1->InterpolateScaling(m_CurrentTime, boneScale1);
	Bone2->InterpolateScaling(m_CurrentTime2, boneScale2);

	finalPos = glm::mix(bonePos1, bonePos2, m_blendAmount);
	finalRot = glm::slerp(boneRot1, boneRot2, m_blendAmount);
	finalRot = glm::normalize(finalRot);
	finalScale = glm::mix(boneScale1, boneScale2, m_blendAmount);

	glm::mat4 translation = glm::translate(glm::mat4(1.0f), finalPos);
	glm::mat4 rotation = glm::toMat4(finalRot);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), finalScale);

	glm::mat4 TRS = glm::mat4(1.0f);
	TRS = translation * rotation * scale;
	return TRS;
}

void SkeletalAnimator::calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* Bone1 = m_CurrentAnimation->findBone(nodeName);
	Bone* Bone2 = NULL;
	if (m_CurrentAnimation2) {
		Bone2 = m_CurrentAnimation2->findBone(nodeName);
	}

	if (Bone1)
	{
		Bone1->Update(m_CurrentTime);
		nodeTransform = Bone1->GetLocalTransform();

		if (Bone2) {
			nodeTransform = updateBlend(Bone1, Bone2);
		}
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;
	m_BoneGlobalTransform[nodeName] = globalTransformation;

	auto boneInfoMap = m_CurrentAnimation->getBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		m_FinalBoneMatrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childrenCount; i++)
		calculateBoneTransform(&node->children[i], globalTransformation);
}

std::vector<glm::mat4> SkeletalAnimator::getFinalBoneMatrices()
{
	return m_FinalBoneMatrices;
}

std::vector<std::string> SkeletalAnimator::getAllNodeNames() {
	const AssimpNodeData* node = &m_CurrentAnimation->getRootNode();
	std::vector<std::string> names;
	getAllNodesNamesHelper(node, names);
	return names;
}

void SkeletalAnimator::getAllNodesNamesHelper(const AssimpNodeData* node, std::vector<std::string>& names) {
	names.emplace_back(node->name);
	for (int i = 0; i < node->childrenCount; i++)
		getAllNodesNamesHelper(&node->children[i], names);
}