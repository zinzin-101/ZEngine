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
	Camera* camera = cam->addComponent<Camera>();
	//camera->farPlane = 10.0f;

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
	ground->transform.scale = glm::vec3(2.25f, 0.25f, 2.25f);
	ground->transform.eulerRotation.x = -90.0f;

	Object* sword = createObject(glm::vec3(1.0f, 0.4f, 0.0f));
	sword->addComponent<Model>(FileSystem::getPath("resources/objects/sword/scene.gltf"));
	sword->transform.scale = glm::vec3(0.065f);
	sword->transform.eulerRotation.y = -90.0f;

	root = createObject(glm::vec3());
	scythe->transform.setParent(&root->transform);
	ground->transform.setParent(&root->transform);
	sword->transform.setParent(&root->transform);
	////cam->transform.setParent(&root->transform);

	Object* smoke1 = createObject(glm::vec3(0.0f, 0.2f, 0.0f));
	smoke1->transform.scale = glm::vec3(16.0f, 32.0f, 16.0f);
	smoke1->addComponent<SmokeSim>(SmokeSimInfo(128, 128, 128));

	//Object* smoke2 = createObject(glm::vec3(0.0f, 0.2f, -2.0f));
	//smoke2->transform.scale = glm::vec3(4.0f, 32.0f, 4.0f);
	//smoke2->addComponent<SmokeSim>(SmokeSimInfo(128, 128, 128));

	//Object* softbody = createObject(glm::vec3(0.0f, 5.0f, 0.0f));
	//softbody->addComponent<GeneralSoftBodyMesh>(FileSystem::getPath("resources/objects/softbody/tetrahedralized_model/cow_tetra.obj"))->shader = renderer->getShader(SHADER_NAME);
	//softbody->transform.scale = glm::vec3(1.0f);
	//GeneralSoftBodyMesh* softbodymesh = softbody->getFirstComponentOfType<GeneralSoftBodyMesh>();
	//softbodymesh->color = glm::vec3(1.0f, 0.0f, 0.0f);
	//softbodymesh->groundHeight = 0.0f;

	//Object* softbody1 = createObject(glm::vec3(0.0f, 5.0f, 0.0f));
	//softbody1->addComponent<GeneralSoftBodyMesh>(FileSystem::getPath("resources/objects/softbody/tetrahedralized_model/icosphere_.obj"))->shader = renderer->getShader(SHADER_NAME);
	//softbody1->transform.scale = glm::vec3(1.0f);
	//softbodymesh = softbody1->getFirstComponentOfType<GeneralSoftBodyMesh>();
	//softbodymesh->color = glm::vec3(1.0f, 0.0f, 0.0f);
	//softbodymesh->groundHeight = 0.0f;

	Object* mountain1 = createObject(glm::vec3(-4.0f, 0.0f, -3.0f));
	mountain1->addComponent<Model>(FileSystem::getPath("resources/objects/mountain/scene.gltf"));
	mountain1->transform.scale = glm::vec3(0.005f);
	mountain1->transform.eulerRotation.y = -180.0f;
	mountain1->transform.setParent(&root->transform);

	Object* mountain2 = createObject(glm::vec3(-4.0f, 0.0f, 0.0f));
	mountain2->addComponent<Model>(FileSystem::getPath("resources/objects/mountain/scene.gltf"));
	mountain2->transform.scale = glm::vec3(0.005f);
	mountain2->transform.eulerRotation.y = -150.0f;

	Object* mountain3 = createObject(glm::vec3(-4.0f, 0.0f, 3.0f));
	mountain3->addComponent<Model>(FileSystem::getPath("resources/objects/mountain/scene.gltf"));
	mountain3->transform.scale = glm::vec3(0.005f);
	mountain3->transform.eulerRotation.y = -210.0f;

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

	if (inputManager.getKeyDown(GLFW_KEY_R)) {
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

	if (inputManager.getKey(GLFW_KEY_UP)) {
		PBRRenderPipeline* pbrPipeline = dynamic_cast<PBRRenderPipeline*>(Engine::getInstance()->getRenderer()->getCurrentRenderPipeline());
		if (pbrPipeline) {
			depthPercentage = pbrPipeline->getDepthPercentage();
			depthPercentage += 0.5f * dt;
			depthPercentage = std::clamp(depthPercentage, 0.0f, 1.0f);
			pbrPipeline->setDepthPercentage(depthPercentage);
			std::cout << depthPercentage << std::endl;
		}
	}
	else if (inputManager.getKey(GLFW_KEY_DOWN)) {
		PBRRenderPipeline* pbrPipeline = dynamic_cast<PBRRenderPipeline*>(Engine::getInstance()->getRenderer()->getCurrentRenderPipeline());
		if (pbrPipeline) {
			depthPercentage = pbrPipeline->getDepthPercentage();
			depthPercentage -= 0.5f * dt;
			depthPercentage = std::clamp(depthPercentage, 0.0f, 1.0f);
			pbrPipeline->setDepthPercentage(depthPercentage);
			std::cout << depthPercentage << std::endl;
		}
	}
}