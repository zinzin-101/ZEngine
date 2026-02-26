#include "Renderer.h"
#include <glad/glad.h>
#include <shader.h>
#include <GLFW/glfw3.h>

Renderer::Renderer() {}

Renderer::~Renderer() {
	for (Shader* shader : shaders) {
		delete shader;
	}
}

bool Renderer::init() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return false;
	}

	glEnable(GL_DEPTH_TEST);

	return true;
}

int Renderer::addShader(Shader* shader){
	int id = shaders.size();
	shaders.emplace_back(shader);
	return id;
}

Shader* Renderer::getShader(int id){
	assert(id >= 0 && id < shaders.size() && "Invalid shader id");
	return shaders.at(id);
}

void Renderer::setViewPort(int x, int y, int width, int height) {
	glViewport(x, y, width, height);
}