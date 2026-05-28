#include "SoftBodyLauncher.h"
#include "../Object.h"
#include "../Engine.h"

SoftBodyLauncher::SoftBodyLauncher(): softbody(nullptr), useUniformLaunch(false), upwardSpeed(10.0f) {}

void SoftBodyLauncher::start() {
	softbody = transform->getObject()->getFirstComponentOfType<GeneralSoftBodyMesh>();
}

void SoftBodyLauncher::update() {
	const std::vector<float>& particlePos = softbody->getParticlePositionsData();
	std::vector<float>& particleVel = softbody->getParticleVelocitiesDataRef();
	std::vector<float>& prevParticlePos = softbody->getParticlePreviousPositionsDataRef();
	int numOfParticles = particlePos.size() / 3;
	bool isTouchingGround = false;
	float dt = Engine::getInstance()->getFixedDeltaTime();
	for (int i = 0; i < numOfParticles; i++) {
		if (particlePos[i * 3 + 1] <= softbody->groundHeight + 0.1f - softbody->getTransform()->position.y) {
			if (particlePos[i * 3 + 1] <= softbody->groundHeight + 0.1f - softbody->getTransform()->position.y) {
				if (useUniformLaunch) {
					isTouchingGround = true;
					break;
				}

				//float speedX = ((rand() / RAND_MAX) * (upwardSpeed / 4.0f)) - (upwardSpeed / 2.0f);
				//float speedZ = ((rand() / RAND_MAX) * (upwardSpeed / 4.0f)) - (upwardSpeed / 2.0f);

				//particleVel[i * 3 + 0] += speedX;
				particleVel[i * 3 + 1] = upwardSpeed;
				//particleVel[i * 3 + 2] += speedZ;
				//prevParticlePos[i * 3 + 0] = particlePos[i * 3 + 0] - particleVel[i * 3 + 0] * dt;
				prevParticlePos[i * 3 + 1] = particlePos[i * 3 + 1] - upwardSpeed * dt;
				//prevParticlePos[i * 3 + 2] = particlePos[i * 3 + 2] - particleVel[i * 3 + 2] * dt;
			}
		}
	}

	if (useUniformLaunch && isTouchingGround) {
		for (int i = 0; i < numOfParticles; i++) {
			//float speedX = ((rand() / RAND_MAX) * (upwardSpeed / 4.0f)) - (upwardSpeed / 2.0f);
			//float speedZ = ((rand() / RAND_MAX) * (upwardSpeed / 4.0f)) - (upwardSpeed / 2.0f);

			//particleVel[i * 3 + 0] += speedX;
			particleVel[i * 3 + 1] = upwardSpeed;
			//particleVel[i * 3 + 2] += speedZ;
			//prevParticlePos[i * 3 + 0] = particlePos[i * 3 + 0] - particleVel[i * 3 + 0] * dt;
			prevParticlePos[i * 3 + 1] = particlePos[i * 3 + 1] - upwardSpeed * dt;
			//prevParticlePos[i * 3 + 2] = particlePos[i * 3 + 2] - particleVel[i * 3 + 2] * dt;
		}
	}
}