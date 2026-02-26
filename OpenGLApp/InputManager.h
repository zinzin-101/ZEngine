#pragma once
#include <map>
#include <glm/glm.hpp>


using GLFW_KEY = unsigned int;

struct GLFWwindow;

class InputManager {
	private:
		GLFWwindow* window;
		std::map<GLFW_KEY, bool> keyDownMap;
		glm::vec2 mousePosition;
		glm::vec2 lastMousePosition;
		glm::vec2 mouseOffset;

	public:
		InputManager();
		void init(GLFWwindow* window);
		bool getKeyDown(GLFW_KEY key);
		bool getKey(GLFW_KEY key);
		void updateMousePosition(glm::vec2 position);
		glm::vec2 getMouseOffset() const;
};