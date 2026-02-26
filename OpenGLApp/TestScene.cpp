#include "TestScene.h"
#include "Camera.h"
#include "Engine.h"
#include "Object.h"
#include "CubePrimitive.h"
#include "PrimitiveMeshRenderer.h"
#include "GLFW/glfw3.h"
#include <iostream>

const std::string CUBE_MESH_NAME = "cube";
const std::string SHADER_NAME = "primitive_shader";

void TestScene::loadMeshData() {
	Renderer* renderer = Engine::getInstance()->getRenderer();

	CubePrimitive* cubeMesh = new CubePrimitive();
	renderer->addMesh(CUBE_MESH_NAME, cubeMesh);

	Shader* shader = new Shader("shaders/primitive.vert", "shaders/primitive.frag");
	renderer->addShader(SHADER_NAME, shader);
}

void TestScene::setup() {
	Renderer* renderer = Engine::getInstance()->getRenderer();

	Object* cam = instantiateObject(glm::vec3(0.0f));
	cam->addComponent<Camera>();
	currentCamera = cam->getFirstComponentOfType<Camera>();

	Object* cube = instantiateObject(glm::vec3(0.0f, 0.0f, -5.0f));
	cube->addComponent<PrimitiveMeshRenderer>();
	PrimitiveMeshRenderer* meshRenderer = cube->getFirstComponentOfType<PrimitiveMeshRenderer>();
	meshRenderer->meshPrimitive = renderer->getMesh(CUBE_MESH_NAME);
	meshRenderer->shader = renderer->getShader(SHADER_NAME);
}

void TestScene::processInput() {
	//std::cout << "from test scene input" << std::endl;

	InputManager& inputManager = *Engine::getInstance()->getInputManager();
	if (inputManager.getKeyDown(GLFW_KEY_ESCAPE)) {
		Engine::getInstance()->terminate();
	}

	float dt = Engine::getInstance()->getTime()->getDeltaTime();
	glm::vec3 movement(0.0f);
	if (inputManager.getKey(GLFW_KEY_W)) {
		movement += currentCamera->getFoward() * 2.5f * dt;
	}
	if (inputManager.getKey(GLFW_KEY_S)) {
		movement += -currentCamera->getFoward() * 2.5f * dt;
	}
	if (inputManager.getKey(GLFW_KEY_A)) {
		movement += -currentCamera->getRight() * 2.5f * dt;
	}
	if (inputManager.getKey(GLFW_KEY_D)) {
		movement += currentCamera->getRight() * 2.5f * dt;
	}
	if (inputManager.getKey(GLFW_KEY_E)) {
		movement += currentCamera->getUp() * 2.5f * dt;
	}
	if (inputManager.getKey(GLFW_KEY_Q)) {
		movement += -currentCamera->getUp() * 2.5f * dt;
	}
	currentCamera->getTransform()->position += movement;
	glm::vec2 mouseOffset = inputManager.getMouseOffset();
	currentCamera->getTransform()->eulerRotation.x -= mouseOffset.y * dt;
	currentCamera->getTransform()->eulerRotation.y += mouseOffset.x * dt;
	if (currentCamera->getTransform()->eulerRotation.x > 89.0f)
		currentCamera->getTransform()->eulerRotation.x = 89.0f;
	if (currentCamera->getTransform()->eulerRotation.x < -89.0f)
		currentCamera->getTransform()->eulerRotation.x = -89.0f;

	if (inputManager.getKeyDown(GLFW_KEY_SPACE)) {
		Engine::getInstance()->getSceneManager()->resetCurrentScene();
	}
}