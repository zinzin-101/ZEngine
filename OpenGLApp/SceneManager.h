#pragma once
#include "Scene.h"

namespace SceneManagerConfig {
	enum SceneId {
		TEST_SCENE,
		TEST_SCENE2,
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