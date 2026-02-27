#include "Scene.h"
#include "Engine.h"
#include "Object.h"

Scene::Scene(): currentCamera(nullptr) {}

Scene::~Scene() {
	for (Object* object : objects) {
		delete object;
	}
	currentCamera = nullptr;
	Engine::getInstance()->getRenderer()->clear();
}

std::vector<Object*>& Scene::getObjects() {
	return objects;
}

void Scene::loadMeshData() {}

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
	loadMeshData();
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

Camera* Scene::getCurrentCamera() {
	if (currentCamera == nullptr) {
		for (Object* object : objects) {
			Camera* camera = object->getFirstComponentOfType<Camera>();
			if (camera != nullptr) {
				currentCamera = camera;
				break;
			}
		}
	}

	return currentCamera;
}