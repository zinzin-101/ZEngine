#include "Component.h"

using namespace ComponentConfig;

int Component::getComponentPriority() const {
	return priority;
}

bool Component::getIsTransparent() const {
	return isTransparent;
}

Component::Component(): priority(DEFAULT_PRIORITY), transform(nullptr), isTransparent(false), isActive(true), autoDeleteOnDestroy(true) {}
Component::~Component() {}

void Component::setTransform(Transform* transform) {
	assert(transform != nullptr && "Setting transform in component is null");
	this->transform = transform;
}

Transform* Component::getTransform() {
	return transform;
}

void Component::init() {}
void Component::start() {}
void Component::preUpdate() {}
void Component::update() {}
void Component::postUpdate() {}
void Component::render() {}

void Component::render(Shader* shader) {
	render();
}

bool compareComponentPriority(const Component* const c1, const Component* const c2) {
	return c1->getComponentPriority() < c2->getComponentPriority();
}
