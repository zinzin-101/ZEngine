#pragma once
#include <GLFW/glfw3.h>
#include <string>

#include "Renderer.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Time.h"

namespace EngineConfig {
	constexpr unsigned int DEFAULT_SCREEN_WIDTH = 1600;
	constexpr unsigned int DEFAULT_SCREEN_HEIGHT = 900;
	inline const char* WINDOW_NAME = "Engine";
	constexpr SceneManagerConfig::SceneId INITIAL_SCENE_ID = SceneManagerConfig::TEST_SCENE;
}

class Engine {
	private:
		static Engine* instance;
		GLFWwindow* window;
		Renderer renderer;
		InputManager inputManager;
		Time time;
		SceneManager sceneManager;

		void initWindow();
		void clean();

		void init();
		void update();

	public:
		Engine();
		~Engine();
		void run();
		void switchScene(SceneManagerConfig::SceneId sceneId);
		static Engine* getInstance();
		Renderer* getRenderer();
		InputManager* getInputManager();
		Time* getTime();
		Scene* getCurrentScene();
		SceneManager* getSceneManager();
		void terminate();
};

void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);