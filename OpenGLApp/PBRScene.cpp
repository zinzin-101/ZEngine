#include "PBRScene.h"

#include "TestScene.h"
#include "Camera.h"
#include "Engine.h"
#include "Object.h"
#include "CubePrimitive.h"
#include "PrimitiveMeshRenderer.h"
#include "SmokeSim.h"
#include "Model.h"
#include "SkeletalModel.h"
#include "SkeletalAnimation.h"
#include "SkeletalAnimator.h"
#include "render_pipelines/PBRRenderPipeline.h"
#include "GeneralSoftBodyMesh.h"
#include "custom/PlayerController.h"
#include "custom/SoftBodyLauncher.h"
#include "custom/SmokeMover.h"
#include "custom/SoftBodyPlayerInteractor.h"
#include "GLFW/glfw3.h"
#include "stb_image.h"
#include <iostream>
#include <filesystem.h>

const std::string CUBE_MESH_NAME = "cube";
const std::string SHADER_NAME = "primitive_shader";

PBRScene::~PBRScene() {
	for (Object* object : objects) {
		object->removeComponent(mountainModelPtr);
		object->removeComponent(churchModelPtr);
	}

	delete mountainModelPtr;
	mountainModelPtr = nullptr;

	delete churchModelPtr;
	churchModelPtr = nullptr;

	std::cout << "cleaning PBR scene" << std::endl;
}

void PBRScene::loadMeshData() {
	Renderer* renderer = Engine::getInstance()->getRenderer();

	CubePrimitive* cubeMesh = new CubePrimitive();
	renderer->addMesh(CUBE_MESH_NAME, cubeMesh);

	Shader* shader = new Shader("shaders/primitive.vert", "shaders/primitive.frag");
	renderer->addShader(SHADER_NAME, shader);
}

void PBRScene::setup() {
	srand(time(NULL));

	Renderer* renderer = Engine::getInstance()->getRenderer();

	Object* cam = instantiateObject(glm::vec3(0.0f));
	freeCam = cam->addComponent<Camera>();
	currentCamera = freeCam;
	//camera->farPlane = 10.0f;

	Object* plrCam = instantiateObject(glm::vec3(0.0f));
	playerCam = plrCam->addComponent<Camera>();

	//currentCamera = cam->getFirstComponentOfType<Camera>();

	//Object* cube = createObject(glm::vec3(0.0f, 0.0f, -5.0f));
	//cube->addComponent<PrimitiveMeshRenderer>();
	//PrimitiveMeshRenderer* meshRenderer = cube->getFirstComponentOfType<PrimitiveMeshRenderer>();
	//meshRenderer->meshPrimitive = renderer->getMesh(CUBE_MESH_NAME);
	//meshRenderer->shader = renderer->getShader(SHADER_NAME);

	//Object* smoke = createObject(glm::vec3(2.0f, 0.0f, 0.0f));
	//smoke->addComponent<SmokeSim>(SmokeSimInfo(128, 128, 128));

	stbi_set_flip_vertically_on_load(false);

	//Object* scythe = createObject(glm::vec3(0.0f, 0.5f, 0.0f));
	//scythe->addComponent<Model>(FileSystem::getPath("resources/objects/scythe/scene.gltf"));
	//scythe->transform.eulerRotation.x = -90.0f;
	//scythe->transform.scale = glm::vec3(0.0075f);

	Object* ground = createObject(glm::vec3(0.0f, 0.05f, 0.0f));
	//ground->addComponent<Model>(FileSystem::getPath("resources/objects/stone_ground/scene.gltf"));
	ground->transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	ground->addComponent<Model>(FileSystem::getPath("resources/objects/gravel_ground/scene.gltf"))->tiling = glm::vec2(20.0f / ground->transform.scale.x);
	//ground->transform.scale = glm::vec3(2.25f, 0.25f, 2.25f);
	//ground->transform.eulerRotation.x = -90.0f;

	//Object* sword = createObject(glm::vec3(1.0f, 0.4f, 0.0f));
	//sword->addComponent<Model>(FileSystem::getPath("resources/objects/sword/scene.gltf"));
	//sword->transform.scale = glm::vec3(0.065f);
	//sword->transform.eulerRotation.y = -90.0f;

	//root = createObject(glm::vec3());
	//scythe->transform.setParent(&root->transform);
	//ground->transform.setParent(&root->transform);
	//sword->transform.setParent(&root->transform);
	////cam->transform.setParent(&root->transform);

	Object* smoke = createObject(glm::vec3(0.0f, 2.5f, 5.0f));
	smoke->transform.scale = glm::vec3(8.0f, 16.0f, 8.0f);
	smoke->addComponent<SmokeSim>(SmokeSimInfo(128, 128, 128));
	//smoke1->addComponent<SmokeMover>();

	smoke = createObject(glm::vec3(0.0f, 0.2f, -2.0f));
	smoke->transform.scale = glm::vec3(4.0f, 16.0f, 4.0f);
	smoke->addComponent<SmokeSim>(SmokeSimInfo(128, 128, 128));

	smoke = createObject(glm::vec3(2.0f, 0.2f, -2.0f));
	smoke->transform.scale = glm::vec3(4.0f, 16.0f, 4.0f);
	smoke->addComponent<SmokeSim>(SmokeSimInfo(128, 128, 128));

	smoke = createObject(glm::vec3(2.0f, 0.2f, 2.0f));
	smoke->transform.scale = glm::vec3(4.0f, 16.0f, 4.0f);
	smoke->addComponent<SmokeSim>(SmokeSimInfo(128, 128, 128));

	//Object* softbody = createObject(glm::vec3(6.0f, 5.0f, 5.0f));
	//softbody->addComponent<GeneralSoftBodyMesh>(FileSystem::getPath("resources/objects/softbody/tetrahedralized_model/cow_tetra.obj"))->shader = renderer->getShader(SHADER_NAME);
	//softbody->transform.scale = glm::vec3(1.0f);
	//GeneralSoftBodyMesh* softbodymesh = softbody->getFirstComponentOfType<GeneralSoftBodyMesh>();
	//softbodymesh->color = glm::vec3(1.0f, 0.0f, 0.0f);
	//softbodymesh->groundHeight = 0.0f;
	
	//SoftBodyLauncher* launcher = softbody->addComponent<SoftBodyLauncher>();
	//launcher->useUniformLaunch = true;

	//Object* softbody1 = createObject(glm::vec3(3.0f, 5.0f, 5.0f));
	//softbody1->addComponent<GeneralSoftBodyMesh>(FileSystem::getPath("resources/objects/softbody/tetrahedralized_model/icosphere_.obj"))->shader = renderer->getShader(SHADER_NAME);
	//softbody1->transform.scale = glm::vec3(1.0f);
	//GeneralSoftBodyMesh* softbodymesh = softbody1->getFirstComponentOfType<GeneralSoftBodyMesh>();
	//softbodymesh->color = glm::vec3(1.0f, 0.0f, 0.0f);
	//softbodymesh->groundHeight = 0.0f;

	//launcher = softbody1->addComponent<SoftBodyLauncher>();
	//launcher->useUniformLaunch = false;
	//launcher->upwardSpeed = 20.0f;

	//mountain1->transform.setParent(&root->transform);

	//Object* mountain2 = createObject(glm::vec3(-4.0f, 0.0f, 0.0f));
	//mountain2->addComponent(mountainModel);
	//mountain2->transform.scale = glm::vec3(0.005f);
	//mountain2->transform.eulerRotation.y = -150.0f;

	//Object* mountain3 = createObject(glm::vec3(-4.0f, 0.0f, 3.0f));
	//mountain3->addComponent(mountainModel);
	//mountain3->transform.scale = glm::vec3(0.005f);
	//mountain3->transform.eulerRotation.y = -210.0f;

	stbi_set_flip_vertically_on_load(true);
	Object* vampire = createObject(glm::vec3(0.0f, 0.0f, -0.5f));
	//vampire->transform.scale = glm::vec3(0.005f);
	vampire->transform.scale = glm::vec3(1.0f);
	vampire->transform.eulerRotation.y = 90.0f;
	SkeletalModel* vampireModel = vampire->addComponent<SkeletalModel>(FileSystem::getPath("resources/objects/vampire/vampire.dae"));
	SkeletalAnimator* vampireAnimator = vampire->addComponent<SkeletalAnimator>();
	player = vampire->addComponent<PlayerController>();
	playerCam->getTransform()->position = vampire->transform.position;
	player->camera = playerCam;

	//vampireAnimator->addAnimation("idle", FileSystem::getPath("resources/objects/mixamo/idle.dae"), vampireModel);
	//vampireAnimator->playAnimation("idle", "", 0.0f, 0.0f, 0.0f);
	stbi_set_flip_vertically_on_load(false);

	churchModelPtr = new Model(FileSystem::getPath("resources/objects/castle_church/scene.gltf"));
	churchModelPtr->autoDeleteOnDestroy = false;

	for (int i = 0; i < 4; i++) {
		float deg = (float)i * 90.0f;
		float rad = glm::radians(deg);
		float r = 25.0f;
		float x = std::cos(rad) * r;
		float z = std::sin(rad) * r;
		Object* castle = createObject(glm::vec3(x, 0.0f, z));
		castle->addComponent(churchModelPtr);
		castle->transform.scale = glm::vec3(1.0f);
		castle->transform.eulerRotation.x = -90.0f;
		castle->transform.eulerRotation.y = deg + 90.0f;
	}

	mountainModelPtr = new Model(FileSystem::getPath("resources/objects/mountain/scene.gltf"));
	mountainModelPtr->autoDeleteOnDestroy = false;

	for (int i = 0; i < 24; i++) {
		float deg = (float)i * (360.0f / 24.0f);
		float rad = glm::radians(deg);
		float r = 30.0f;
		float x = std::cos(rad) * r;
		float z = std::sin(rad) * r;
		Object* mountain = createObject(glm::vec3(x, -0.1f + ((rand() / RAND_MAX) * 0.05f), z));
		mountain->addComponent(mountainModelPtr);
		mountain->transform.scale = glm::vec3(0.01f + ((rand() / RAND_MAX) * 0.005f));
		mountain->transform.eulerRotation.y = deg - 180.0f;
	}

	Object* treeHolder = createObject(glm::vec3(2.0f, 0.0f, 4.0f));
	Object* tree = createObject(glm::vec3(0.0f, -0.5f, 0.0f));
	tree->transform.setParent(&treeHolder->transform);
	tree->transform.scale = glm::vec3(0.15f);
	tree->addComponent<Model>(FileSystem::getPath("resources/objects/dark_tree/scene.gltf"));

	//Object* house1 = createObject(glm::vec3(-6.0f, 0.0f, 10.0f));
	//house1->addComponent<Model>(FileSystem::getPath("resources/objects/traditional_japanese_house/scene.gltf"));
	//house1->transform.scale = glm::vec3(0.015f);

	//Object* chisa = createObject(glm::vec3(-2.0f, 0.0f, 0.0f));
	//chisa->addComponent<Model>(FileSystem::getPath("resources/objects/chisa/scene.gltf"));
	//chisa->transform.eulerRotation.x = -90.0f;
	//chisa->transform.scale = glm::vec3(0.0075f);

	//SoftBodyPlayerInteractor* softBodyInteractor = softbody->addComponent<SoftBodyPlayerInteractor>();
	//softBodyInteractor->playerTransform = &vampire->transform;
	//softBodyInteractor->playerRadius = 1.0f;
	//softBodyInteractor->pushForce = 20.0f;

	for (int i = 0; i < 9; i++) {
		float x = ((rand() / RAND_MAX) * 2.0f) - 4.0f;
		float y = 4.0f;
		float z = ((rand() / RAND_MAX) * 2.0f) - 4.0f;
		Object* softbody1 = createObject(glm::vec3(0.0f + i * 0.1f, y, 0.0f));
		softbody1->addComponent<GeneralSoftBodyMesh>(FileSystem::getPath("resources/objects/softbody/tetrahedralized_model/icosphere_.obj"))->shader = renderer->getShader(SHADER_NAME);
		softbody1->transform.scale = glm::vec3(1.0f);
		GeneralSoftBodyMesh* softbodymesh = softbody1->getFirstComponentOfType<GeneralSoftBodyMesh>();
		softbodymesh->color = glm::vec3(0.8f, 0.8f, 0.8f);
		softbodymesh->groundHeight = 0.0f;

		SoftBodyPlayerInteractor* softBodyInteractor = softbody1->addComponent<SoftBodyPlayerInteractor>();
		softBodyInteractor->playerTransform = &vampire->transform;
		softBodyInteractor->playerRadius = 1.0f;
		softBodyInteractor->pushForce = 20.0f;
	}

}

void PBRScene::processInput() {
	//std::cout << "current cam: " << (currentCamera == freeCam ? "free" : "player") << std::endl;

	//std::cout << "from test scene input" << std::endl;

	InputManager& inputManager = *Engine::getInstance()->getInputManager();
	if (inputManager.getKeyDown(GLFW_KEY_ESCAPE)) {
		Engine::getInstance()->terminate();
	}

	if (inputManager.getKeyDown(GLFW_KEY_SPACE)) {
		float timeScale = Engine::getInstance()->getTime()->timeScale;
		Engine::getInstance()->getTime()->timeScale = timeScale > 0.5f ? 0.0f : 1.0f;
	}

	float dt = Engine::getInstance()->getTime()->getDeltaTime();
	glm::vec3 movement(0.0f);
	glm::vec2 playerMovement(0.0f);
	if (inputManager.getKey(GLFW_KEY_W)) {
		movement += currentCamera->getFoward() * 2.5f * dt;
		playerMovement.y += 1.0f;
	}
	if (inputManager.getKey(GLFW_KEY_S)) {
		movement += -currentCamera->getFoward() * 2.5f * dt;
		playerMovement.y -= 1.0f;
	}
	if (inputManager.getKey(GLFW_KEY_A)) {
		movement += -currentCamera->getRight() * 2.5f * dt;
		playerMovement.x -= 1.0f;
	}
	if (inputManager.getKey(GLFW_KEY_D)) {
		movement += currentCamera->getRight() * 2.5f * dt;
		playerMovement.x += 1.0f;
	}
	if (inputManager.getKey(GLFW_KEY_E)) {
		movement += currentCamera->getUp() * 2.5f * dt;
	}
	if (inputManager.getKey(GLFW_KEY_Q)) {
		movement += -currentCamera->getUp() * 2.5f * dt;
	}

	if (currentCamera == playerCam) {
		Engine::getInstance()->setEnableCursor(false);
		glm::vec2 mouseOffset = inputManager.getMouseOffset();
		currentCamera->getTransform()->eulerRotation.x -= mouseOffset.y * 5.0f * dt;
		currentCamera->getTransform()->eulerRotation.y += mouseOffset.x * 5.0f * dt;
		if (currentCamera->getTransform()->eulerRotation.x > 89.0f)
			currentCamera->getTransform()->eulerRotation.x = 89.0f;
		if (currentCamera->getTransform()->eulerRotation.x < -89.0f)
			currentCamera->getTransform()->eulerRotation.x = -89.0f;
		player->move(playerMovement);
	}
	else if (currentCamera == freeCam) {
		if (inputManager.getKey(GLFW_KEY_LEFT_SHIFT)) {
			currentCamera->getTransform()->position += movement * 10.0f;
		}
		else {
			currentCamera->getTransform()->position += movement;
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
	}

	if (inputManager.getKey(GLFW_KEY_RIGHT)) {
		//root->transform.eulerRotation.y += 15.0f * dt;
		//if (inputManager.getKey(GLFW_KEY_LEFT_SHIFT)) {
		//	root->transform.eulerRotation.y += 50.0f * dt;
		//}
		PBRRenderPipeline* pbrPipeline = dynamic_cast<PBRRenderPipeline*>(Engine::getInstance()->getRenderer()->getCurrentRenderPipeline());
		if (pbrPipeline) {
			float angle = pbrPipeline->getBackgroundRotateAngle();
			if (inputManager.getKey(GLFW_KEY_LEFT_SHIFT)) {
				angle += 50.0f * dt;
			}
			else {
				angle += 15.0f * dt;
			}
			pbrPipeline->setBackgroundRotateAngle(angle);
		}
	}
	else if (inputManager.getKey(GLFW_KEY_LEFT)) {
		//root->transform.eulerRotation.y -= 15.0f * dt;
		//if (inputManager.getKey(GLFW_KEY_LEFT_SHIFT)) {
		//	root->transform.eulerRotation.y -= 50.0f * dt;
		//}
		PBRRenderPipeline* pbrPipeline = dynamic_cast<PBRRenderPipeline*>(Engine::getInstance()->getRenderer()->getCurrentRenderPipeline());
		if (pbrPipeline) {
			float angle = pbrPipeline->getBackgroundRotateAngle();
			if (inputManager.getKey(GLFW_KEY_LEFT_SHIFT)) {
				angle -= 50.0f * dt;
			}
			else {
				angle -= 15.0f * dt;
			}
			pbrPipeline->setBackgroundRotateAngle(angle);
		}
	}

	if (inputManager.getKeyDown(GLFW_KEY_R)) {
		Engine::getInstance()->getSceneManager()->resetCurrentScene();
	}

	if (inputManager.getKeyDown(GLFW_KEY_V)) {
		Camera* lastCam = currentCamera;
		currentCamera = currentCamera == playerCam ? freeCam : playerCam;
		currentCamera->getTransform()->position = lastCam->getTransform()->position;
		currentCamera->getTransform()->eulerRotation = lastCam->getTransform()->eulerRotation;
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
	else if (inputManager.getKeyDown(GLFW_KEY_3)) {
		PBRRenderPipeline* pbrPipeline = dynamic_cast<PBRRenderPipeline*>(Engine::getInstance()->getRenderer()->getCurrentRenderPipeline());
		if (pbrPipeline) {
			pbrPipeline->setEnvironmentMap("resources/textures/hdr/morning_2k.hdr");
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