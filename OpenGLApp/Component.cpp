#include "Component.h"

using namespace ComponentConfig;

int Component::getComponentPriority() const {
	return priority;
}

Component::Component(): priority(DEFAULT_PRIORITY), transform(nullptr) {}
Component::~Component() {}

void Component::setTransform(Transform* transform) {
	assert(transform != nullptr && "Setting transform in component is null");
	this->transform = transform;
}

void Component::init() {}
void Component::start() {}
void Component::update() {}
void Component::render() {}

bool compareComponentPriority(const Component* const c1, const Component* const c2) {
	return c1->getComponentPriority() < c2->getComponentPriority();
}
