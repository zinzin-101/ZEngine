#include "Scene.h"
#include "Engine.h"
#include "Object.h"

Scene::Scene(): currentCamera(nullptr) {}

Scene::~Scene() {
	for (Object* object : objects) {
		delete object;
	}
}

std::vector<Object*>& Scene::getObjects() {
	return objects;
}

void Scene::setup() {}

void Scene::init() {
	for (Object* object : objects) {
		object->setCurrentScene(this);
		object->init();
	}
}

void Scene::start() {
	for (Object* object : objects) {
		object->start();
	}
}

void Scene::prepareScene() {
	setup();
	init();
	start();
}

void Scene::update() {
	for (Object* object : objects) {
		object->update();
	}

	for (std::vector<Object*>::iterator itr = objects.end(); itr != objects.begin();) {
		--itr;
		Object* object = *itr;
		if (object->getCanDelete()) {
			delete object;
			itr = objects.erase(itr);
		}
	}
}

void Scene::render() {
	for (Object* object : objects) {
		object->render();
	}
}

void Scene::processInput() {}

Object* Scene::instantiateObject(glm::vec3 position) {
	Object* object = new Object();
	object->setCurrentScene(this);
	object->transform.position = position;
	objects.emplace_back(object);
	return object;
}