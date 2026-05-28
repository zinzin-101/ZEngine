#include "PlayerController.h"
#include "../Object.h"
#include "../Engine.h"
#include <filesystem.h>
#include <iostream>

PlayerController::PlayerController() :
	moveSpeed(5.0f),
	camera(nullptr),
	animator(nullptr),
	model(nullptr),
	currentState(IDLE),
	blendAmount(0.0f)
{}

void PlayerController::start() {
	animator = transform->getObject()->getFirstComponentOfType<SkeletalAnimator>();
	model = transform->getObject()->getFirstComponentOfType<SkeletalModel>();

	if (camera == nullptr) {
		camera = Engine::getInstance()->getCurrentScene()->getCurrentCamera();
	}

	animator->addAnimation("idle", FileSystem::getPath("resources/objects/mixamo/idle.dae"), model);
	animator->addAnimation("jog", FileSystem::getPath("resources/objects/mixamo/jog.dae"), model);


	animator->playAnimation("idle", "", 0.0f, 0.0f, 0.0f);
}

void PlayerController::update() {
	const float CAM_DIST = 5.0f;
	const float CAM_HEIGHT = 1.0f;

	glm::vec3 camPos = transform->position;
	camPos -= camera->getFoward() * CAM_DIST;
	if (camPos.y < 0.1f) {
		camPos.y = 0.1f;
	}
	camPos += camera->getUp() * CAM_HEIGHT;

	camera->getTransform()->position = camPos;

	const float blendRate = 0.055f;
	switch (currentState) {
		case IDLE:
			blendAmount = 0.0f;
			break;

		case IDLE_JOG:
			blendAmount += blendRate;
			blendAmount = fmod(blendAmount, 1.0f);
			animator->playAnimation("idle", "jog", animator->m_CurrentTime, animator->m_CurrentTime2, blendAmount);
			if (blendAmount > 0.9f) {
				blendAmount = 0.0f;
				float startTime = animator->m_CurrentTime2;
				animator->playAnimation("jog", "", startTime, 0.0f, blendAmount);
				currentState = JOG;
			}
			break;

		case JOG:
			blendAmount = 0.0f;
			break;

		case JOG_IDLE:
			blendAmount += blendRate;
			blendAmount = fmod(blendAmount, 1.0f);
			animator->playAnimation("jog", "idle", animator->m_CurrentTime, animator->m_CurrentTime2, blendAmount);
			if (blendAmount > 0.9f) {
				blendAmount = 0.0f;
				float startTime = animator->m_CurrentTime2;
				animator->playAnimation("idle", "", startTime, 0.0f, blendAmount);
				currentState = IDLE;
			}
			break;
	}
}

void PlayerController::move(glm::vec2 input) {

	//std::cout << "transform->eulerRotation.y: " << transform->eulerRotation.y << std::endl;

	glm::vec3 forward = camera->getFoward() * input.y;
	glm::vec3 right = camera->getRight() * input.x;

	glm::vec3 result = forward + right;
	result.y = 0.0f;
	if (glm::length(result) < 0.01f) {
		if (currentState == JOG) {
			currentState = JOG_IDLE;
			animator->playAnimation("jog", "idle", animator->m_CurrentTime, animator->m_CurrentTime2, blendAmount);
		}
		return;
	}

	float dt = Engine::getInstance()->getDeltaTime();
	result = glm::normalize(result);
	if (currentState == IDLE) {
		currentState = IDLE_JOG;
		animator->playAnimation("idle", "jog", animator->m_CurrentTime, animator->m_CurrentTime2, blendAmount);
	}
	float targetAngle = glm::degrees(std::atan2(result.x, result.z));
	//transform->eulerRotation.y = targetAngle;

	float delta = targetAngle - transform->eulerRotation.y;
	delta = std::fmod(delta + 180.0f, 360.0f) - 180.0f;

	const float ROTATION_SPEED = 10.0f;
	transform->eulerRotation.y += delta * (std::min)(1.0f, ROTATION_SPEED * dt);
	float rad = glm::radians(transform->eulerRotation.y);
	glm::vec3 steeringDir = glm::vec3(std::sin(rad), 0.0f, std::cos(rad));

	transform->position += steeringDir * moveSpeed * dt;
	//transform->position += result * moveSpeed * dt;
}