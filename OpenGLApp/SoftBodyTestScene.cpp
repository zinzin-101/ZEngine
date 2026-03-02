#include "SoftBodyTestScene.h"
#include "Camera.h"
#include "Engine.h"
#include "Object.h"
#include "CubePrimitive.h"
#include "PrimitiveMeshRenderer.h"
#include "TetrahedronSoftBodyMesh.h"
#include "GLFW/glfw3.h"
#include <iostream>

const std::string CUBE_MESH_NAME = "cube";
const std::string SHADER_NAME = "primitive_shader";

void SoftBodyTestScene::loadMeshData() {
	Renderer* renderer = Engine::getInstance()->getRenderer();

	CubePrimitive* cubeMesh = new CubePrimitive();
	renderer->addMesh(CUBE_MESH_NAME, cubeMesh);

	Shader* shader = new Shader("shaders/primitive.vert", "shaders/primitive.frag");
	renderer->addShader(SHADER_NAME, shader);
}

void SoftBodyTestScene::setup() {
	Renderer* renderer = Engine::getInstance()->getRenderer();

	Engine::getInstance()->getTime()->timeScale = 0.0f;

	Object* cam = instantiateObject(glm::vec3(0.0f, 5.0f, 5.0f));
	cam->transform.eulerRotation.x = -30.0f;
	cam->addComponent<Camera>();
	//currentCamera = cam->getFirstComponentOfType<Camera>();

	Object* cube = instantiateObject(glm::vec3(0.0f, 0.0f, 0.0f));
	cube->transform.scale = glm::vec3(10.0f, 1.0f, 10.0f);
	cube->addComponent<PrimitiveMeshRenderer>();
	PrimitiveMeshRenderer* meshRenderer = cube->getFirstComponentOfType<PrimitiveMeshRenderer>();
	meshRenderer->meshPrimitive = renderer->getMesh(CUBE_MESH_NAME);
	meshRenderer->shader = renderer->getShader(SHADER_NAME);
	meshRenderer->color = glm::vec3(0.5f);

	//meshRenderer->isActive = false;

	Object* softbody = instantiateObject(glm::vec3(0.0f, 5.0f, 0.0f));
	softbody->addComponent<TetrahedronSoftBodyMesh>()->shader = renderer->getShader(SHADER_NAME);
	softbody->getFirstComponentOfType<TetrahedronSoftBodyMesh>()->groundHeight = 0.5f;

}

void SoftBodyTestScene::processInput() {
	float dtRealTime =  Engine::getInstance()->getTime()->getRealDeltaTime();
	//std::cout << "realtime: " << dtRealTime << std::endl;

	InputManager& inputManager = *Engine::getInstance()->getInputManager();
	if (inputManager.getKeyDown(GLFW_KEY_ESCAPE)) {
		Engine::getInstance()->terminate();
	}

	float dt = Engine::getInstance()->getTime()->getDeltaTime();
	//std::cout << "game time: " << dt << std::endl;
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

	currentCamera->getTransform()->position += movement * 5.0f;
	glm::vec2 mouseOffset = inputManager.getMouseOffset();
	currentCamera->getTransform()->eulerRotation.x -= mouseOffset.y * 5.0f * dt;
	currentCamera->getTransform()->eulerRotation.y += mouseOffset.x * 5.0f * dt;
	if (currentCamera->getTransform()->eulerRotation.x > 89.0f)
		currentCamera->getTransform()->eulerRotation.x = 89.0f;
	if (currentCamera->getTransform()->eulerRotation.x < -89.0f)
		currentCamera->getTransform()->eulerRotation.x = -89.0f;

	if (inputManager.getKeyDown(GLFW_KEY_SPACE)) {
		Engine::getInstance()->getSceneManager()->resetCurrentScene();
	}

	if (inputManager.getKeyDown(GLFW_KEY_X)) {
		Time* time = Engine::getInstance()->getTime();
		if (time->timeScale > 0.5f) {
			time->timeScale = 0.0f;
		}
		else {
			time->timeScale = 1.0f;
		}
	}
}