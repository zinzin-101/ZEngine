#include "Ray.h"
#include "Engine.h"

Ray::Ray(glm::vec3 origin, glm::vec3 direction, float distance): origin(origin), direction(direction), distance(distance) {}

float Ray::getDistanceFromPoint(glm::vec3 point) {
	glm::vec3 dir = glm::normalize(direction);
	glm::vec3 op = point - origin;
	float t = glm::dot(op, dir);
	t = glm::clamp(t, 0.0f, distance);

	glm::vec3 closest = origin + (dir * t);
	return glm::distance(point, closest);
}

glm::vec3 Ray::getRayDirectionFromScreen(glm::vec2 mousePosition, Camera* camera) {
	//glm::vec2 screenDimension = Engine::getInstance()->getScreenDimension();
	//float x = (2.0f * mousePosition.x) / screenDimension.x - 1.0f;
	//float y = 1.0f - (2.0f * mousePosition.y) / screenDimension.y;
	//float z = 1.0f;
	//glm::vec3 rayNDC = glm::vec3(x, y, z);
	//glm::vec4 rayClip = glm::vec4(rayNDC.x, rayNDC.y, -1.0f, 1.0f);
	//glm::vec4 rayView = glm::inverse(camera->getProjectionMatrix()) * rayClip;
	//rayView = glm::vec4(rayView.x, rayView.y, -1.0, 0.0);
	//glm::vec3 rayWorld = glm::vec3(glm::inverse(camera->getViewMatrix()) * rayView);
	//return glm::normalize(rayWorld);
	Engine* engine = Engine::getInstance();

	// Get world point at near plane (z=0)
	glm::vec3 nearPoint = engine->screenToWorld(mousePosition, 0.0f,
		camera->getProjectionMatrix(), camera->getViewMatrix());

	// Get world point at far plane (z=1)
	glm::vec3 farPoint = engine->screenToWorld(mousePosition, 1.0f,
		camera->getProjectionMatrix(), camera->getViewMatrix());

	return glm::normalize(farPoint - nearPoint);
}