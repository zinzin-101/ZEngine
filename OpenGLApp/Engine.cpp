#include "Engine.h"
#include "SceneManager.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cassert>
#include <stdexcept>

//#include <filesystem.h>

using namespace EngineConfig;

Engine* Engine::instance = nullptr;

Engine* Engine::getInstance() {
	assert(instance != nullptr && "Engine instance is null");
	return instance;
}

Renderer* Engine::getRenderer() {
	return &renderer;
}

InputManager* Engine::getInputManager() {
	return &inputManager;
}

Time* Engine::getTime() {
	return &time;
}

Scene* Engine::getCurrentScene() {
	return sceneManager.getCurrentScene();
}

SceneManager* Engine::getSceneManager() {
	return &sceneManager;
}

Engine::Engine(): window(nullptr), screenWidth(DEFAULT_SCREEN_WIDTH), screenHeight(DEFAULT_SCREEN_HEIGHT) {
	assert(instance == nullptr && "Trying to create more than one Engine instance");
	instance = this;
	init();
}
Engine::~Engine() {
	clean();
}

void Engine::init() {
	initWindow();
	
	if (!renderer.init()) {
		throw std::runtime_error("Failed to init renderer");
	}

	inputManager.init(window);
}

void Engine::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	if (INIT_IN_FULL_SCREEN) {
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
		screenWidth = mode->width;
		screenHeight = mode->height;
		window = glfwCreateWindow(screenWidth, screenHeight, WINDOW_NAME, primaryMonitor, NULL);
	}
	else {
		window = glfwCreateWindow(screenWidth, screenHeight, WINDOW_NAME, NULL, NULL);
	}

	if (window == nullptr) {
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window");
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
}

void Engine::clean() {
	glfwTerminate();
}

void Engine::update() {
	inputManager.update();
	time.update();
	sceneManager.update();
	renderer.render();
}

void Engine::run() {
	switchScene(INITIAL_SCENE_ID);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		update();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Engine::switchScene(SceneManagerConfig::SceneId sceneId) {
	sceneManager.switchSceneFromId(sceneId);
}

glm::vec2 Engine::getScreenDimension() const {
	return glm::vec2(screenWidth, screenHeight);
}

void Engine::terminate() {
	glfwSetWindowShouldClose(window, true);
}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
	Engine* engine = Engine::getInstance();
	if (engine != nullptr) {
		engine->getRenderer()->setViewPort(0, 0, width, height);
	}
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	Engine* engine = Engine::getInstance();
	if (engine != nullptr) {
		engine->getInstance()->getInputManager()->updateMousePosition(glm::vec2(xpos, ypos));
	}
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	processInput(window);
}

void processInput(GLFWwindow* window) {
	Engine* engine = Engine::getInstance();
	if (engine != nullptr) {
		Scene* scene = engine->getInstance()->getCurrentScene();
		if (scene != nullptr && scene->hasBeenInitialized()) {
			scene->processInput();
		}
	}
}