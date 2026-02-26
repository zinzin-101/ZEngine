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

void SceneManager::updateScene() {
	if (currentScene != nullptr) {
		currentScene->update();
	}
}

void SceneManager::switchSceneFromId(SceneId sceneId) {
	currentSceneId = sceneId;
	if (prevSceneId == currentSceneId) return;

	clearCurrentScene();

	switch (sceneId) {
		case SceneId::TEST_SCENE:
			currentScene = new TestScene();
		default:
			return;
	}

	currentScene->prepareScene();
}

Scene* SceneManager::getCurrentScene() {
	return currentScene;
}