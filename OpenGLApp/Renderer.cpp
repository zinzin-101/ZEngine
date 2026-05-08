#include "Renderer.h"
#include <glad/glad.h>
#include <shader.h>
#include <GLFW/glfw3.h>
#include "Engine.h"
#include "Scene.h"
#include "Object.h"

using namespace RendererOperation;

Renderer::Renderer() {}

Renderer::~Renderer() {
	clear();
}

bool Renderer::init() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return false;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

		// save previous GL states
		GLboolean powerWasEnabled;
		GLint prevSrcRGB, prevDstRGB, prevSrcAlpha, prevDstAlpha;
		GLboolean prevDepthMask;

		powerWasEnabled = glIsEnabled(GL_BLEND);
		glGetIntegerv(GL_BLEND_SRC_RGB, &prevSrcRGB);
		glGetIntegerv(GL_BLEND_DST_RGB, &prevDstRGB);
		glGetIntegerv(GL_BLEND_SRC_ALPHA, &prevSrcAlpha);
		glGetIntegerv(GL_BLEND_DST_ALPHA, &prevDstAlpha);
		glGetBooleanv(GL_DEPTH_WRITEMASK, &prevDepthMask);

		// apply GL states for transparency rendering
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);

		while (!transparencyRenderQueue.empty()) {
			TransparentComponentRendering transparentComponent = transparencyRenderQueue.top();
			transparencyRenderQueue.pop();
			transparentComponent.component->render();
		}

		// restore previous GL states
		powerWasEnabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
		glBlendFuncSeparate(prevSrcRGB, prevDstRGB, prevSrcAlpha, prevDstAlpha);
		glDepthMask(prevDepthMask);
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

void Renderer::addToTransparencyQueue(Component* component) {
	glm::vec3 cameraPos = component->getTransform()->getObject()->getCurrentScene()->getCurrentCamera()->getTransform()->getGlobalPosition();
	glm::vec3 componentPos = component->getTransform()->getGlobalPosition();
	transparencyRenderQueue.emplace(component, glm::distance(cameraPos, componentPos));
}

void Renderer::clear() {
	clearMesh();
	clearShader();
}

void Renderer::setViewPort(int x, int y, int width, int height) {
	glViewport(x, y, width, height);
}

RendererOperation::TransparentComponentRendering::TransparentComponentRendering(Component* component, float distanceToCamera) :
	component(component), distanceToCamera(distanceToCamera) {
}

bool RendererOperation::TransparencyComparator::operator()(const TransparentComponentRendering& c1, const TransparentComponentRendering& c2) {
	return c1.distanceToCamera < c2.distanceToCamera;
}