#pragma once
#include "Scene.h"

namespace SceneManagerConfig {
	enum SceneId {
		TEST_SCENE,
		SOFT_BODY_TEST_SCENE,
		NONE
	};
}

class SceneManager {
	private:
		SceneManagerConfig::SceneId currentSceneId;
		SceneManagerConfig::SceneId prevSceneId;
		Scene* currentScene;
		void clearCurrentScene();
		void updateScene();
		void manageScene();

	public:
		SceneManager();
		~SceneManager();
		void update();
		void switchSceneFromId(SceneManagerConfig::SceneId sceneId);
		void resetCurrentScene();
		Scene* getCurrentScene();
		SceneManagerConfig::SceneId getCurrentSceneId() const;
};