#include "InputManager.h"
#include "Engine.h"
#include <GLFW/glfw3.h>
#include <cassert>

InputManager::InputManager(): window(nullptr) {
	mousePosition = glm::vec2(EngineConfig::DEFAULT_SCREEN_WIDTH / 2.0f, EngineConfig::DEFAULT_SCREEN_HEIGHT / 2.0f);
	lastMousePosition = glm::vec2(EngineConfig::DEFAULT_SCREEN_WIDTH / 2.0f, EngineConfig::DEFAULT_SCREEN_HEIGHT / 2.0f);
	mouseOffset = glm::vec2(0.0f);
}

void InputManager::init(GLFWwindow* window) {
	this->window = window;
}

bool InputManager::getKeyDown(GLFW_KEY key) {
	assert(window != nullptr && "window is null in input manager");

	// init
	if (keyDownMap.count(key) == 0) {
		keyDownMap[key] = false;
		return false;
	}

	if (glfwGetKey(window, key) == GLFW_PRESS && keyDownMap.at(key)) {
		return false;
	}

	if (glfwGetKey(window, key) == GLFW_RELEASE && keyDownMap.at(key)) {
		keyDownMap[key] = false;
		return false;
	}

	if (glfwGetKey(window, key) == GLFW_PRESS && !keyDownMap.at(key)) {
		keyDownMap[key] = true;
		return true;
	}

	return false;
}

bool InputManager::getKey(GLFW_KEY key) {
	assert(window != nullptr && "window is null in input manager");
	return glfwGetKey(window, key) == GLFW_PRESS;
}

void InputManager::updateMousePosition(glm::vec2 position) {
	static bool firstMouseMovement = true;

	mousePosition = position;

	if (firstMouseMovement) {
		firstMouseMovement = false;
		lastMousePosition = mousePosition;
	}

	mouseOffset.x = mousePosition.x - lastMousePosition.x;
	mouseOffset.y = mousePosition.y - lastMousePosition.y; // reversed since y-coordinates go from bottom to top

	lastMousePosition = mousePosition;
}

glm::vec2 InputManager::getMouseOffset() const {
	return mouseOffset;
}