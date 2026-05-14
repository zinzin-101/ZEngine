#include "Camera.h"
#include "Engine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <limits>

using namespace CameraConfig;

Camera::Camera(): fov(DEFAULT_FOV), nearPlane(DEFAULT_NEAR_PLANE), farPlane(DEFAULT_FAR_PLANE) {
	forward = glm::vec3(0.0f, 0.0f, -1.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	worldUp = up;
	right = glm::vec3(1.0f, 0.0f, 0.0f);
}

void Camera::setForward(glm::vec3 forward) {
	this->forward = glm::normalize(forward);
	right = glm::normalize(glm::cross(this->forward, worldUp));
	up = glm::normalize(glm::cross(right, this->forward));
}

glm::mat4 Camera::getViewMatrix() const {
	glm::vec3 position = transform->getGlobalPosition();
	return glm::lookAt(position, position + forward, up);
}

glm::mat4 Camera::getProjectionMatrix() const {
	glm::vec2 screenDimension = Engine::getInstance()->getScreenDimension();
	return glm::perspective(glm::radians(fov),
		(float)screenDimension.x / (float)screenDimension.y,
		nearPlane, farPlane);
}

glm::vec3 Camera::getFoward() const {
	return forward;
}

glm::vec3 Camera::getRight() const {
	return right;
}

glm::vec3 Camera::getUp() const {
	return up;
}

void Camera::getMinMaxShadowCorners(const glm::mat4& lightView, glm::vec3& min, glm::vec3& max) const {
	glm::vec3 minCorner = glm::vec3((std::numeric_limits<float>::max)());
	glm::vec3 maxCorner = glm::vec3((std::numeric_limits<float>::lowest)());

	// calculate each corners
	int idx = 0;
	glm::mat4 invVP = glm::inverse(getProjectionMatrix() * getViewMatrix());
	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 2; y++) {
			for (int z = 0; z < 2; z++) {
				glm::vec4 pt = invVP * glm::vec4(
					2.0f * x - 1.0f,
					2.0f * y - 1.0f,
					2.0f * z - 1.0f,
					1.0f
				);
				glm::vec3 corner = glm::vec3(pt) / pt.w;
				glm::vec3 lightSpaceCorner = glm::vec3(lightView * glm::vec4(corner, 1.0f));

				minCorner.x = (glm::min)(minCorner.x, lightSpaceCorner.x);
				minCorner.y = (glm::min)(minCorner.y, lightSpaceCorner.y);
				minCorner.z = (glm::min)(minCorner.z, lightSpaceCorner.z);

				maxCorner.x = (glm::max)(maxCorner.x, lightSpaceCorner.x);
				maxCorner.y = (glm::max)(maxCorner.y, lightSpaceCorner.y);
				maxCorner.z = (glm::max)(maxCorner.z, lightSpaceCorner.z);
			}
		}
	}

	min = minCorner;
	max = maxCorner;
}

void Camera::updateCameraVector() {
	assert(transform != nullptr && "Transform is null in camera");

	glm::vec3 tempForward;
	// x: pitch, y: yaw, z: roll
	float pitch = transform->eulerRotation.x;
	float yaw = transform->eulerRotation.y - 90.0f;
	float roll = transform->eulerRotation.z;
	tempForward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	tempForward.y = sin(glm::radians(pitch));
	tempForward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward = glm::normalize(tempForward);

	right = glm::normalize(glm::cross(forward, worldUp));
	up = glm::normalize(glm::cross(right, forward));
}

void Camera::init() {
	updateCameraVector();
}

void Camera::update() {
	updateCameraVector();
}