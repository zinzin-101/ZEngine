#pragma once
#include "Scene.h"

namespace SceneManagerConfig {
	enum class SceneId {
		TEST_SCENE,
		NONE
	};
}

class SceneManager {
	private:
		SceneManagerConfig::SceneId currentSceneId;
		SceneManagerConfig::SceneId prevSceneId;
		Scene* currentScene;
		void clearCurrentScene();

	public:
		SceneManager();
		~SceneManager();
		void updateScene();
		void switchSceneFromId(SceneManagerConfig::SceneId sceneId);
		Scene* getCurrentScene();
};