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
	currentState(IDLE)
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
	if (camPos.y < 0.0f) {
		camPos.y = 0.0f;
	}
	camPos += camera->getUp() * CAM_HEIGHT;

	camera->getTransform()->position = camPos;
}

void PlayerController::move(glm::vec2 input) {

	//std::cout << "transform->eulerRotation.y: " << transform->eulerRotation.y << std::endl;

	glm::vec3 forward = camera->getFoward() * input.y;
	glm::vec3 right = camera->getRight() * input.x;

	glm::vec3 result = forward + right;
	result.y = 0.0f;
	if (glm::length(result) < 0.01f) {
		if (currentState != IDLE) {
			animator->playAnimation("idle", "", 0.0f, 0.0f, 0.0f);
		}
		currentState = IDLE;
		return;
	}

	float dt = Engine::getInstance()->getDeltaTime();
	result = glm::normalize(result);
	transform->position += result * dt;
	if (currentState != JOG) {
		animator->playAnimation("jog", "", 0.0f, 0.0f, 0.0f);
	}
	currentState = JOG;
	float targetAngle = glm::degrees(std::atan2(result.x, result.z));
	transform->eulerRotation.y = targetAngle;
}