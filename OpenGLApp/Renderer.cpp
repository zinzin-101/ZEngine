#include "Renderer.h"
#include <glad/glad.h>
#include <shader.h>
#include <GLFW/glfw3.h>
#include "Engine.h"
#include "Scene.h"
#include "Object.h"

Renderer::Renderer() {}

Renderer::~Renderer() {}

bool Renderer::init() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return false;
	}

	glEnable(GL_DEPTH_TEST);

	return true;
}

void Renderer::render() {
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Scene* scene = Engine::getInstance()->getCurrentScene();
	if (scene != nullptr) {
		std::vector<Object*>& objects = scene->getObjects();
		for (Object* object : objects) {
			object->render();
		}
	}
}

void Renderer::addMesh(std::string name, MeshPrimitive* meshPrimitive) {
	assert(!nameToMeshPrimitive.contains(name) && "Trying to add an already existing mesh primitive");
	nameToMeshPrimitive[name] = meshPrimitive;
}

MeshPrimitive* Renderer::getMesh(std::string name) {
	assert(nameToMeshPrimitive.contains(name) && "Mesh with this name does not exist");
	return nameToMeshPrimitive.at(name);
}

void Renderer::clearMesh() {
	for (std::pair<std::string, MeshPrimitive*> pair : nameToMeshPrimitive) {
		delete pair.second;
	}

	nameToMeshPrimitive.clear();
}

void Renderer::addShader(std::string name, Shader* shader) {
	assert(!nameToShader.contains(name) && "Trying to add an already existing shader");
	nameToShader[name] = shader;
}


Shader* Renderer::getShader(std::string name) {
	assert(nameToShader.contains(name) && "shader with this name does not exist");
	return nameToShader.at(name);
}

void Renderer::clearShader() {
	for (std::pair<std::string, Shader*> pair : nameToShader) {
		delete pair.second;
	}

	nameToShader.clear();
}

void Renderer::clear() {
	clearMesh();
	clearShader();
}

void Renderer::setViewPort(int x, int y, int width, int height) {
	glViewport(x, y, width, height);
}