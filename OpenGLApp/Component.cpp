#include "Component.h"

using namespace ComponentConfig;

int Component::getComponentPriority() const {
	return priority;
}

Component::Component(): priority(DEFAULT_PRIORITY) {}
Component::~Component() {}

bool compareComponentPriority(const Component* const c1, const Component* const c2) {
	return c1->getComponentPriority() < c2->getComponentPriority();
}
