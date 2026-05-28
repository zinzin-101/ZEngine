#include "SmokeMover.h"
#include "../Object.h"
#include "../Engine.h"

SmokeMover::SmokeMover(): sim(nullptr), t(0.0f), angularSpeed(10.0f), radius(5.0f), origin(0.0f) {}

void SmokeMover::start() {
	sim = transform->getObject()->getFirstComponentOfType<SmokeSim>();
	origin = transform->position;
}

void SmokeMover::update() {
	float dt = Engine::getInstance()->getDeltaTime();

	t += dt;

	glm::vec3 pos = sim->getTransform()->position;
	glm::vec3 prevPos = pos;
	pos.x = radius * std::cos(t) + origin.x;
	pos.z = radius * std::sin(t) + origin.z;

	sim->getTransform()->position = pos;

	pos.y = 0.0f;
	prevPos.y = 0.0f;
	glm::vec3 displacement = pos - prevPos;
	sim->smokeDirection = -displacement;
	sim->smokeAcceleration = glm::length(displacement) * 10.0f;
}