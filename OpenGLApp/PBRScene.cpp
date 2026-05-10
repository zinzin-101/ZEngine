#include "PBRScene.h"

#include "TestScene.h"
#include "Camera.h"
#include "Engine.h"
#include "Object.h"
#include "CubePrimitive.h"
#include "PrimitiveMeshRenderer.h"
#include "SmokeSim.h"
#include "Model.h"
#include "render_pipelines/PBRRenderPipeline.h"
#include "GeneralSoftBodyMesh.h"
#include "GLFW/glfw3.h"
#include "stb_image.h"
#include <iostream>
#include <filesystem.h>

const std::string CUBE_MESH_NAME = "cube";
const std::string SHADER_NAME = "primitive_shader";

void PBRScene::loadMeshData() {
	Renderer* renderer = Engine::getInstance()->getRenderer();

	CubePrimitive* cubeMesh = new CubePrimitive();
	renderer->addMesh(CUBE_MESH_NAME, cubeMesh);

	Shader* shader = new Shader("shaders/primitive.vert", "shaders/primitive.frag");
	renderer->addShader(SHADER_NAME, shader);
}

void PBRScene::setup() {
	Renderer* renderer = Engine::getInstance()->getRenderer();

	Object* cam = instantiateObject(glm::vec3(0.0f));
	cam->addComponent<Camera>();
	//currentCamera = cam->getFirstComponentOfType<Camera>();

	//Object* cube = createObject(glm::vec3(0.0f, 0.0f, -5.0f));
	//cube->addComponent<PrimitiveMeshRenderer>();
	//PrimitiveMeshRenderer* meshRenderer = cube->getFirstComponentOfType<PrimitiveMeshRenderer>();
	//meshRenderer->meshPrimitive = renderer->getMesh(CUBE_MESH_NAME);
	//meshRenderer->shader = renderer->getShader(SHADER_NAME);

	//Object* smoke = createObject(glm::vec3(2.0f, 0.0f, 0.0f));
	//smoke->addComponent<SmokeSim>(SmokeSimInfo(128, 128, 128));

	stbi_set_flip_vertically_on_load(false);

	Object* scythe = createObject(glm::vec3(0.0f, 0.5f, 0.0f));
	scythe->addComponent<Model>(FileSystem::getPath("resources/objects/scythe/scene.gltf"));
	scythe->transform.eulerRotation.x = -90.0f;
	scythe->transform.scale = glm::vec3(0.0075f);

	Object* ground = createObject(glm::vec3(0.0f, -0.1f, 0.0f));
	ground->addComponent<Model>(FileSystem::getPath("resources/objects/stone_ground/scene.gltf"));
	ground->transform.scale = glm::vec3(1.05f, 0.25f, 1.05f);
	ground->transform.eulerRotation.x = -90.0f;

	Object* sword = createObject(glm::vec3(1.0f, 0.4f, 0.0f));
	sword->addComponent<Model>(FileSystem::getPath("resources/objects/sword/scene.gltf"));
	sword->transform.scale = glm::vec3(0.065f);
	sword->transform.eulerRotation.y = -90.0f;

	root = createObject(glm::vec3());
	scythe->transform.setParent(&root->transform);
	ground->transform.setParent(&root->transform);
	sword->transform.setParent(&root->transform);
	//cam->transform.setParent(&root->transform);

	Object* smoke = createObject(glm::vec3(0.0f, 0.2f, 0.0f));
	smoke->addComponent<SmokeSim>(SmokeSimInfo(128, 128, 128));

	Object* softbody = createObject(glm::vec3(0.0f, 5.0f, 0.0f));
	softbody->addComponent<GeneralSoftBodyMesh>(FileSystem::getPath("resources/objects/softbody/tetrahedralized_model/cow_tetra.obj"))->shader = renderer->getShader(SHADER_NAME);
	softbody->transform.scale = glm::vec3(1.0f);
	GeneralSoftBodyMesh* softbodymesh = softbody->getFirstComponentOfType<GeneralSoftBodyMesh>();
	softbodymesh->color = glm::vec3(1.0f, 0.0f, 0.0f);
	softbodymesh->groundHeight = 0.0f;

	Object* softbody1 = createObject(glm::vec3(0.0f, 5.0f, 0.0f));
	softbody->addComponent<GeneralSoftBodyMesh>(FileSystem::getPath("resources/objects/softbody/tetrahedralized_model/icosphere_.obj"))->shader = renderer->getShader(SHADER_NAME);
	softbody->transform.scale = glm::vec3(1.0f);
	softbodymesh = softbody->getFirstComponentOfType<GeneralSoftBodyMesh>();
	softbodymesh->color = glm::vec3(1.0f, 0.0f, 0.0f);
	softbodymesh->groundHeight = 0.0f;

	//Object* chisa = createObject(glm::vec3(-2.0f, 0.0f, 0.0f));
	//chisa->addComponent<Model>(FileSystem::getPath("resources/objects/chisa/scene.gltf"));
	//chisa->transform.eulerRotation.x = -90.0f;
	//chisa->transform.scale = glm::vec3(0.0075f);
}

void PBRScene::processInput() {
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

	if (inputManager.getKey(GLFW_KEY_LEFT_SHIFT)) {
		currentCamera->getTransform()->position += movement * 2.0f;
	}
	else {
		currentCamera->getTransform()->position += movement;
	}

	if (inputManager.getKey(GLFW_KEY_RIGHT)) {
		root->transform.eulerRotation.y += 15.0f * dt;
		if (inputManager.getKey(GLFW_KEY_LEFT_SHIFT)) {
			root->transform.eulerRotation.y += 50.0f * dt;
		}
	}
	else if (inputManager.getKey(GLFW_KEY_LEFT)) {
		root->transform.eulerRotation.y -= 15.0f * dt;
		if (inputManager.getKey(GLFW_KEY_LEFT_SHIFT)) {
			root->transform.eulerRotation.y -= 50.0f * dt;
		}
	}

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
		Engine::getInstance()->getSceneManager()->resetCurrentScene();
	}

	if (inputManager.getKeyDown(GLFW_KEY_1)) {
		PBRRenderPipeline* pbrPipeline = dynamic_cast<PBRRenderPipeline*>(Engine::getInstance()->getRenderer()->getCurrentRenderPipeline());
		if (pbrPipeline) {
			pbrPipeline->setEnvironmentMap("resources/textures/hdr/newport_loft.hdr");
		}
	}
	else if (inputManager.getKeyDown(GLFW_KEY_2)) {
		PBRRenderPipeline* pbrPipeline = dynamic_cast<PBRRenderPipeline*>(Engine::getInstance()->getRenderer()->getCurrentRenderPipeline());
		if (pbrPipeline) {
			pbrPipeline->setEnvironmentMap("resources/textures/hdr/studio.hdr");
		}
	}

	if (inputManager.getKeyDown(GLFW_KEY_T)) {
		PBRRenderPipeline* pbrPipeline = dynamic_cast<PBRRenderPipeline*>(Engine::getInstance()->getRenderer()->getCurrentRenderPipeline());
		if (pbrPipeline) {
			pbrPipeline->setUseDepthOfField(!pbrPipeline->isUsingDepthOfField());
		}
	}
}