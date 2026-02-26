#include "Object.h"
#include "Scene.h"

Object::Object(): currentScene(nullptr), transform(this), name("Object"), canDelete(false) {}

Object::Object(std::string name): currentScene(nullptr), transform(this), name(name) {}

Object::~Object() {
	for (Component* component : components) {
		delete component;
	}
}

void Object::init() {
	start();
}

void Object::start() {}

void Object::update() {}

void Object::setCurrentScene(Scene* scene) {
	currentScene = scene;
}

bool Object::getCanDelete() const {
	return canDelete;
}

void Object::deleteObject(Object* object) {
	object->canDelete = true;
}