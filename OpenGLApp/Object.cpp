#include "Object.h"
#include "Scene.h"

Object::Object(): currentScene(nullptr), transform(this), name("Object"), canDelete(false) {}

Object::Object(std::string name): currentScene(nullptr), transform(this), name(name), canDelete(false) {}

Object::~Object() {
	for (Component* component : components) {
		delete component;
	}
}

void Object::init() {
	for (Component* component : components) {
		component->init();
	}
}

void Object::start() {
	for (Component* component : components) {
		component->start();
	}
}

void Object::update() {
	for (Component* component : components) {
		component->update();
	}
}

void Object::render() {
	for (Component* component : components) {
		component->render();
	}
}

void Object::setCurrentScene(Scene* scene) {
	currentScene = scene;
}

void Object::addComponent(Component* component) {
	component->setTransform(&transform);
	components.emplace_back(component);
	std::sort(components.begin(), components.end(), compareComponentPriority);
}

void Object::removeComponent(Component* component) {
	for (std::vector<Component*>::iterator itr = components.end(); itr != components.begin();) {
		itr--;
		Component* currentComponent = *itr;
		if (currentComponent == component) {
			delete currentComponent;
			components.erase(itr);
			break;
		}
	}
}

bool Object::getCanDelete() const {
	return canDelete;
}

void Object::deleteObject(Object* object) {
	object->canDelete = true;
}