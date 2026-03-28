#pragma once
//#include <GLFW/glfw3.h>
#include <string>

#include "Renderer.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Time.h"

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

		friend void frameBufferSizeCallback(GLFWwindow* window, int width, int height);

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
		GLFWwindow* getWindow();
		glm::vec2 getScreenDimension() const;
		void setEnableCursor(bool value);
		void terminate();

		glm::vec3 screenToWorld(glm::vec2 screenPosition, float depth, glm::mat4 projection, glm::mat4 view);
};

void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);