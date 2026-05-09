#include "Renderer.h"
#include <glad/glad.h>
#include <shader.h>
#include <GLFW/glfw3.h>
#include "Engine.h"
#include "EngineConfig.h"
#include "Scene.h"
#include "Object.h"

using namespace RendererOperation;

Renderer::Renderer(): currentRenderPipeline(nullptr) {}

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

	setCurrentRenderPipeline(getRenderPipeline(EngineConfig::INITIAL_RENDERING_PIPELINE));

	return true;
}

void Renderer::render() {
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Scene* scene = Engine::getInstance()->getCurrentScene();
	if (scene != nullptr) {
		std::vector<Object*>& objects = scene->getObjects();
		currentRenderPipeline->render(objects);
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

void Renderer::clearTransparencyQueue() {
	transparencyRenderQueue = std::priority_queue<
		RendererOperation::TransparentComponentRendering,
		std::vector<RendererOperation::TransparentComponentRendering>,
		RendererOperation::TransparencyComparator
	>();
}

std::priority_queue<
	RendererOperation::TransparentComponentRendering,
	std::vector<RendererOperation::TransparentComponentRendering>,
	RendererOperation::TransparencyComparator
>& Renderer::getTransparencyQueue() {
	return transparencyRenderQueue;
}

void Renderer::setCurrentRenderPipeline(RenderPipeline* pipeline) {
	if (currentRenderPipeline != nullptr) {
		delete currentRenderPipeline;
		currentRenderPipeline = nullptr;
	}

	currentRenderPipeline = pipeline;
	currentRenderPipeline->init();
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

#include "render_pipelines/SimpleRenderPipeline.h"

RenderPipeline* Renderer::getRenderPipeline(RenderPipelines pipeline) {
	switch (pipeline) {
		case SIMPLE:
			return new SimpleRenderPipeline();
	}

	return nullptr;
}