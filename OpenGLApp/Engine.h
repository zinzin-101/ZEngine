#pragma once
//#include <GLFW/glfw3.h>
#include <string>

#include "Renderer.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Time.h"

namespace EngineConfig {
	constexpr unsigned int DEFAULT_SCREEN_WIDTH = 1600;
	constexpr unsigned int DEFAULT_SCREEN_HEIGHT = 900;
	constexpr bool INIT_IN_FULL_SCREEN = false;
	inline const char* WINDOW_NAME = "ZEngine";
	constexpr SceneManagerConfig::SceneId INITIAL_SCENE_ID = SceneManagerConfig::SOFT_BODY_TEST_SCENE;
	constexpr float FIXED_DT = 1.0f / 144.0f;
}

struct GLFWwindow;

class Engine {
	private:
		static Engine* instance;
		GLFWwindow* window;
		Renderer renderer;
		InputManager inputManager;
		Time time;
		SceneManager sceneManager;

		unsigned int screenWidth;
		unsigned int screenHeight;

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
		glm::vec2 getScreenDimension() const;
		void terminate();
};

void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);