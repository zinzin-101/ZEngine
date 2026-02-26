#include "Camera.h"

Camera::Camera(Transform* transform): Component(transform) {
	forward = glm::vec3(0.0f, 0.0f, -1.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	worldUp = up;
	update();
}

void Camera::setForward(glm::vec3 forward) {
	this->forward = glm::normalize(forward);
	right = glm::normalize(glm::cross(this->forward, worldUp));
	up = glm::normalize(glm::cross(right, this->forward));
}

glm::mat4 Camera::getViewMatrix() const {
	glm::vec3 position = transform->position;
	return glm::lookAt(position, position + forward, up);
}

void Camera::update() {
	assert(transform != nullptr && "Transform is null in camera");

	glm::vec3 tempForward;
	// x: pitch, y: yaw, z: roll
	float pitch = transform->eulerRotation.x;
	float yaw = transform->eulerRotation.y;
	float roll = transform->eulerRotation.z;
	tempForward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	tempForward.y = sin(glm::radians(pitch));
	tempForward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward = glm::normalize(tempForward);

	right = glm::normalize(glm::cross(forward, worldUp));
	up = glm::normalize(glm::cross(right, forward));
}