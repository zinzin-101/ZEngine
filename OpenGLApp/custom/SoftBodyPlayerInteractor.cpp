#include "SoftBodyPlayerInteractor.h"
#include "../Object.h"

SoftBodyPlayerInteractor::SoftBodyPlayerInteractor(): playerTransform(nullptr), softbody(nullptr), playerRadius(0.5f), pushForce(5.0f) {}

void SoftBodyPlayerInteractor::start() {
	softbody = transform->getObject()->getFirstComponentOfType<GeneralSoftBodyMesh>();
}

void SoftBodyPlayerInteractor::update() {
	if (playerTransform == nullptr) {
		std::cout << "Player is null" << std::endl;
		return;
	}

	std::vector<float>& particlePos = softbody->getParticlePositionsDataRef();
	std::vector<float>& prevParticlePos = softbody->getParticlePreviousPositionsDataRef();
	std::vector<float>& particleVel = softbody->getParticleVelocitiesDataRef();
	int n = particlePos.size() / 3;
	glm::vec3 softbodyOrigin = softbody->getTransform()->position;
	for (int i = 0; i < n; i++) {
		glm::vec3 pos = glm::vec3(
			particlePos[i * 3 + 0] + softbodyOrigin.x,
			particlePos[i * 3 + 1] + softbodyOrigin.y,
			particlePos[i * 3 + 2] + softbodyOrigin.z
		);

		glm::vec3 displacement = pos - playerTransform->position;
		float dist = glm::length(displacement);

		if (dist > playerRadius)
			continue;

		if (dist < 1e-6f)
			continue;

		glm::vec3 pushDir = glm::normalize(displacement);
		glm::vec3 newPos = playerTransform->position + pushDir * playerRadius;
		glm::vec3 pushDelta = newPos - pos;

		float oldLocalX = particlePos[i * 3 + 0];
		float oldLocalY = particlePos[i * 3 + 1];
		float oldLocalZ = particlePos[i * 3 + 2];

		particlePos[i * 3 + 0] = newPos.x - softbodyOrigin.x;
		particlePos[i * 3 + 1] = newPos.y - softbodyOrigin.y;
		particlePos[i * 3 + 2] = newPos.z - softbodyOrigin.z;

		prevParticlePos[i * 3 + 0] = oldLocalX;
		prevParticlePos[i * 3 + 1] = oldLocalY;
		prevParticlePos[i * 3 + 2] = oldLocalZ;

		float penetrationDepth = playerRadius - dist;
		float scaledForce = pushForce * (penetrationDepth / playerRadius);
		glm::vec3 pushVelocity = pushDir * scaledForce;

		particleVel[i * 3 + 0] = pushDelta.x + pushVelocity.x;
		particleVel[i * 3 + 1] = pushDelta.y + pushVelocity.y;
		particleVel[i * 3 + 2] = pushDelta.z + pushVelocity.z;
	}
}