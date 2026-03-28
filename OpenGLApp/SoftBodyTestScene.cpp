#include "SoftBodyTestScene.h"
#include "Camera.h"
#include "Engine.h"
#include "Object.h"
#include "CubePrimitive.h"
#include <filesystem.h>
#include "PrimitiveMeshRenderer.h"
#include "TetrahedronSoftBodyMesh.h"
#include "GeneralSoftBodyMesh.h"
#include "Ray.h"
#include "GLFW/glfw3.h"
#include <iostream>

const std::string CUBE_MESH_NAME = "cube";
const std::string SHADER_NAME = "primitive_shader";

SoftBodyTestScene::ModelSelect SoftBodyTestScene::modelSelect = SoftBodyTestScene::ModelSelect ::ICOSPHERE;

void SoftBodyTestScene::loadMeshData() {
	Renderer* renderer = Engine::getInstance()->getRenderer();

	CubePrimitive* cubeMesh = new CubePrimitive();
	renderer->addMesh(CUBE_MESH_NAME, cubeMesh);

	Shader* shader = new Shader("shaders/primitive.vert", "shaders/primitive.frag");
	renderer->addShader(SHADER_NAME, shader);

	modelPaths[ICOSPHERE] = FileSystem::getPath("resources/objects/softbody/tetrahedralized_model/icosphere_.obj");
	modelPaths[ARMADILLO] = FileSystem::getPath("resources/objects/softbody/tetrahedralized_model/armadillo_tetra_20k.obj");
	modelPaths[MONKEY] = FileSystem::getPath("resources/objects/softbody/tetrahedralized_model/tetrahedron_monkey.obj");
	modelPaths[COW] = FileSystem::getPath("resources/objects/softbody/tetrahedralized_model/cow_tetra.obj");

	constraints[ICOSPHERE] = { 0.0408936f, 0.172566f };
	constraints[ARMADILLO] = { 0.0001f, 0.0001f};
	constraints[MONKEY] = { 2.5f, 2.5f };
	constraints[COW] = { 7.5f, 7.5f };
}

void SoftBodyTestScene::setup() {
	dragIndex = -1;

	Engine::getInstance()->setEnableCursor(true);

	Renderer* renderer = Engine::getInstance()->getRenderer();

	Engine::getInstance()->getTime()->timeScale = 1.0f;

	Object* cam = instantiateObject(glm::vec3(0.0f, 5.0f, 5.0f));
	cam->transform.eulerRotation.x = -30.0f;
	cam->addComponent<Camera>();
	//currentCamera = cam->getFirstComponentOfType<Camera>();

	Object* cube = instantiateObject(glm::vec3(0.0f, 0.0f, 0.0f));
	cube->transform.scale = glm::vec3(50.0f, 1.0f, 50.0f);
	cube->addComponent<PrimitiveMeshRenderer>();
	PrimitiveMeshRenderer* meshRenderer = cube->getFirstComponentOfType<PrimitiveMeshRenderer>();
	meshRenderer->meshPrimitive = renderer->getMesh(CUBE_MESH_NAME);
	meshRenderer->shader = renderer->getShader(SHADER_NAME);
	meshRenderer->color = glm::vec3(1.0f);

	//meshRenderer->isActive = false;

	Object* softbody = instantiateObject(glm::vec3(0.0f, 5.0f, 0.0f));
	//Object* softbody2 = instantiateObject(glm::vec3(0.0f, 5.0f, 5.0f));
	//softbody2->addComponent<GeneralSoftBodyMesh>(FileSystem::getPath("resources/objects/softbody/armadillo_tetra_20k.obj"))->shader = renderer->getShader(SHADER_NAME);
	//softbodymesh = softbody2->getFirstComponentOfType<GeneralSoftBodyMesh>();
	//softbodymesh->color = glm::vec3(0.0f, 0.0f, 1.0f);
	//softbodymesh->groundHeight = 0.5f;
	//softbodymesh->edgeCompliance = 50.0f;
	//softbodymesh->volumeCompliance = 50.0f;

	//softbody->addComponent<TetrahedronSoftBodyMesh>()->shader = renderer->getShader(SHADER_NAME);
	//softbodymesh = softbody->getFirstComponentOfType<TetrahedronSoftBodyMesh>();
	//softbody->addComponent<GeneralSoftBodyMesh>(FileSystem::getPath("resources/objects/softbody/tetrahedralized_model/icosphere_.obj"))->shader = renderer->getShader(SHADER_NAME);
	//softbody->addComponent<GeneralSoftBodyMesh>(FileSystem::getPath("resources/objects/softbody/tetrahedralized_model/icosphere_.obj"))->shader = renderer->getShader(SHADER_NAME);
	//softbody->addComponent<GeneralSoftBodyMesh>(FileSystem::getPath("resources/objects/softbody/tetrahedralized_model/armadillo_tetra_20k.obj"))->shader = renderer->getShader(SHADER_NAME);
	//softbody->addComponent<GeneralSoftBodyMesh>(FileSystem::getPath("resources/objects/softbody/tetrahedralized_model/tetrahedron_monkey.obj"))->shader = renderer->getShader(SHADER_NAME);
	softbody->addComponent<GeneralSoftBodyMesh>(modelPaths[modelSelect])->shader = renderer->getShader(SHADER_NAME);
	softbodymesh = softbody->getFirstComponentOfType<GeneralSoftBodyMesh>();
	softbodymesh->color = glm::vec3(1.0f, 0.0f, 0.0f);
	softbodymesh->groundHeight = 0.5f;
	//softbodymesh->edgeCompliance = 0.172566f;
	//softbodymesh->volumeCompliance = 0.0408936f;
	//softbodymesh->edgeCompliance = 50.0f;
	//softbodymesh->volumeCompliance = 50.0f;
	//softbodymesh->edgeCompliance = 0.00001f;
	//softbodymesh->volumeCompliance = 0.00001f;

	softbodymesh->volumeCompliance = constraints[modelSelect].volumeCompliance;
	softbodymesh->edgeCompliance = constraints[modelSelect].edgeCompliance;

	//softbodyMesh->edgeCompliance;
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

	if (inputManager.getMouse(GLFW_MOUSE_BUTTON_RIGHT)) {
		Engine::getInstance()->setEnableCursor(false);

		glm::vec2 mouseOffset = inputManager.getMouseOffset();
		currentCamera->getTransform()->eulerRotation.x -= mouseOffset.y * 5.0f * dt;
		currentCamera->getTransform()->eulerRotation.y += mouseOffset.x * 5.0f * dt;
		if (currentCamera->getTransform()->eulerRotation.x > 89.0f)
			currentCamera->getTransform()->eulerRotation.x = 89.0f;
		if (currentCamera->getTransform()->eulerRotation.x < -89.0f)
			currentCamera->getTransform()->eulerRotation.x = -89.0f;
	}
	else {
		Engine::getInstance()->setEnableCursor(true);
	}

	if (inputManager.getKeyDown(GLFW_KEY_SPACE)) {
		//Engine::getInstance()->getSceneManager()->resetCurrentScene();
		softbodymesh->pauseSimulation = !softbodymesh->pauseSimulation;
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

	if (inputManager.getKey(GLFW_KEY_UP)) {
		softbodymesh->volumeCompliance += 1.0f * dt;
	}
	if (inputManager.getKey(GLFW_KEY_DOWN)) {
		softbodymesh->volumeCompliance -= 1.0f * dt;
	}

	if (inputManager.getKey(GLFW_KEY_RIGHT)) {
		softbodymesh->edgeCompliance += 1.0f * dt;
	}
	if (inputManager.getKey(GLFW_KEY_LEFT)) {
		softbodymesh->edgeCompliance -= 1.0f * dt;
	}

	if (inputManager.getKeyDown(GLFW_KEY_R)) {
		//softbodymesh->reset();
		Engine::getInstance()->getSceneManager()->resetCurrentScene();
	}

	if (inputManager.getKeyDown(GLFW_KEY_1)) {
		modelSelect = ModelSelect::ICOSPHERE;
		Engine::getInstance()->getSceneManager()->resetCurrentScene();
	}
	else if (inputManager.getKeyDown(GLFW_KEY_2)) {
		modelSelect = ModelSelect::ARMADILLO;
		Engine::getInstance()->getSceneManager()->resetCurrentScene();
	}
	else if (inputManager.getKeyDown(GLFW_KEY_3)) {
		modelSelect = ModelSelect::MONKEY;
		Engine::getInstance()->getSceneManager()->resetCurrentScene();
	}
	else if (inputManager.getKeyDown(GLFW_KEY_4)) {
		modelSelect = ModelSelect::COW;
		Engine::getInstance()->getSceneManager()->resetCurrentScene();
	}

	if (inputManager.getKeyDown(GLFW_KEY_V)) {
		softbodymesh->renderAsWireframe = !softbodymesh->renderAsWireframe;
	}

	softbodymesh->volumeCompliance = glm::clamp(softbodymesh->volumeCompliance, 0.0f, 100.0f);
	softbodymesh->edgeCompliance = glm::clamp(softbodymesh->edgeCompliance, 0.0f, 100.0f);
	//std::cout << "volume compliance: " << softbodymesh->volumeCompliance << std::endl;
	//std::cout << "edge compliance: " << softbodymesh->edgeCompliance << std::endl;

	glm::vec3 position = softbodymesh->getTransform()->getGlobalPosition();
	glm::vec3 rotation = softbodymesh->getTransform()->getGlobalEulerRotation();
	glm::vec3 scale = softbodymesh->getTransform()->getGlobalScale();
	glm::mat4 model(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, scale);
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, -1.0f));

	if (dragIndex == -1 && inputManager.getMouseDown(GLFW_MOUSE_BUTTON_LEFT)) {
		//std::cout << "enter" << std::endl;
		const std::vector<float>& softbodyPositionData = softbodymesh->getParticlePositionsData();
		int n = softbodyPositionData.size() / 3;
		glm::vec2 mousePosition = inputManager.getMousePosition();
		glm::vec3 nearPoint = Engine::getInstance()->screenToWorld(mousePosition, 0.0f,
			currentCamera->getProjectionMatrix(), currentCamera->getViewMatrix());
		Ray ray = Ray(nearPoint, Ray::getRayDirectionFromScreen(inputManager.getMousePosition(), currentCamera), 100.0f);
		float closestDistance = FLT_MAX;
		int closestIndex = -1;
		for (int i = 0; i < n; i++) {
			glm::vec3 pos = glm::vec3(
				softbodyPositionData.at(i * 3 + 0),
				softbodyPositionData.at(i * 3 + 1),
				softbodyPositionData.at(i * 3 + 2)
			);

			pos = model * glm::vec4(pos, 1.0f);

			float distance = ray.getDistanceFromPoint(pos);
			if (distance < closestDistance) {
				closestDistance = distance;
				closestIndex = i;
			}
		}

		if (closestIndex >= 0) {
			dragIndex = closestIndex;

			lastInvMass = softbodymesh->getInverseMass(dragIndex);
			softbodymesh->setInverseMass(dragIndex, 0.0f);

			glm::vec3 pos = glm::vec3(
				softbodyPositionData.at(dragIndex * 3 + 0),
				softbodyPositionData.at(dragIndex * 3 + 1),
				softbodyPositionData.at(dragIndex * 3 + 2));

			pos = model * glm::vec4(pos, 1.0f);

			glm::vec4 clipPos = currentCamera->getProjectionMatrix() * currentCamera->getViewMatrix() * glm::vec4(pos, 1.0f);
			dragDepth = (clipPos.z / clipPos.w) * 0.5f + 0.5f;
		}
	}

	if (dragIndex >= 0 && inputManager.getMouse(GLFW_MOUSE_BUTTON_LEFT)) {
		//std::cout << "stay index: " << dragIndex << std::endl;
		glm::vec2 mousePos = inputManager.getMousePosition();

		const std::vector<float>& softbodyPositionData = softbodymesh->getParticlePositionsData();
		glm::vec3 worldPos = glm::vec3(
			softbodyPositionData.at(dragIndex * 3 + 0),
			softbodyPositionData.at(dragIndex * 3 + 1),
			softbodyPositionData.at(dragIndex * 3 + 2)
		);

		worldPos = Engine::getInstance()->screenToWorld(mousePos, dragDepth, currentCamera->getProjectionMatrix(), currentCamera->getViewMatrix());

		glm::mat4 invModel = glm::inverse(model);
		glm::vec3 pos = glm::vec3(invModel * glm::vec4(worldPos, 1.0f));

		softbodymesh->setParticlePosition(dragIndex, pos);
	}

	if (dragIndex >= 0 && glfwGetMouseButton(Engine::getInstance()->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		softbodymesh->setInverseMass(dragIndex, lastInvMass);
		dragIndex = -1;
	}

	//std::cout << dragIndex << std::endl;
}