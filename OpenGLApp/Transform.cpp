#include "Transform.h"
#include "Object.h"

Transform::Transform(Object* owner): object(owner), parent(nullptr) {
	position = glm::vec3(0.0f);
	eulerRotation = glm::vec3(0.0f);
	scale = glm::vec3(1.0f);
};

Object* Transform::getObject() {
	return object;
}

glm::vec3 Transform::getGlobalPosition() const {
	if (parent != nullptr) {
		return position + parent->getGlobalPosition();
	}
	return position;
}

glm::vec3 Transform::getGlobalEulerRotation() const {
	if (parent != nullptr) {
		return eulerRotation + parent->getGlobalEulerRotation();
	}
	return eulerRotation;
}

glm::vec3 Transform::getGlobalScale() const {
	if (parent != nullptr) {
		return scale * parent->getGlobalScale();
	}
	return scale;
}

glm::mat4 Transform::getGlobalModelMatrix() const {
	glm::vec3 position = getGlobalPosition();
	glm::vec3 rotation = getGlobalEulerRotation();
	glm::vec3 scale = getGlobalScale();

	glm::mat4 model(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, scale);
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, -1.0f));

	return model;
}

void Transform::removeSelfFromParent(Transform* parent) {
	assert(parent != nullptr && "Trying to remove from a null parent");
	for (std::vector<Transform*>::iterator itr = parent->children.begin(); itr != parent->children.begin();) {
		itr--;
		Transform* child = *itr;
		if (child == this) {
			parent->children.erase(itr);
			break;
		}
	}
}

void Transform::setParent(Transform* parent) {
	if (this->parent == parent) return;

	if (parent == nullptr) {
		if (this->parent != nullptr) {
			removeSelfFromParent(this->parent);
		}
		this->parent = nullptr;
		return;
	}

	if (this->parent != nullptr) {
		removeSelfFromParent(this->parent);
		this->parent = nullptr;
	}

	this->parent = parent;
	parent->children.emplace_back(this);
}