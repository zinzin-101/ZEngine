#pragma once
#include <map>
#include <glm/glm.hpp>


using GLFW_KEY = unsigned int;

struct GLFWwindow;

class InputManager {
	private:
		GLFWwindow* window;
		std::map<GLFW_KEY, bool> keyDownMap;
		std::map<GLFW_KEY, bool> mouseDownMap;
		std::map<GLFW_KEY, bool> mouseUpMap;
		glm::vec2 mousePosition;
		glm::vec2 lastMousePosition;
		glm::vec2 mouseOffset;

	public:
		InputManager();
		void init(GLFWwindow* window);
		void update();
		bool getKeyDown(GLFW_KEY key);
		bool getKey(GLFW_KEY key);
		bool getMouseDown(GLFW_KEY button);
		bool getMouse(GLFW_KEY button);
		void updateMousePosition(glm::vec2 position);
		glm::vec2 getMousePosition() const;
		glm::vec2 getMouseOffset() const;
};