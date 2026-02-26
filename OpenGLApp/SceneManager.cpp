#include "SceneManager.h"
#include "TestScene.h"

using namespace SceneManagerConfig;

SceneManager::SceneManager(): currentSceneId(SceneId::NONE), prevSceneId(SceneId::NONE), currentScene(nullptr) {}


SceneManager::~SceneManager() {
	clearCurrentScene();
}

void SceneManager::clearCurrentScene() {
	if (currentScene != nullptr) {
		delete currentScene;
		currentScene = nullptr;
	}
}

void SceneManager::update() {
	manageScene();
	updateScene();
}

void SceneManager::manageScene() {
	if (prevSceneId != currentSceneId) {
		clearCurrentScene();

		switch (currentSceneId) {
			case SceneId::TEST_SCENE:
				currentScene = new TestScene();
		}

		currentScene->prepareScene();
	}
	prevSceneId = currentSceneId;
}

void SceneManager::updateScene() {
	if (currentScene != nullptr) {
		currentScene->update();
	}
}

void SceneManager::switchSceneFromId(SceneId sceneId) {
	if (sceneId < 0 || sceneId >= NONE) return;
	currentSceneId = sceneId;
}

Scene* SceneManager::getCurrentScene() {
	return currentScene;
}