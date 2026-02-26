#include "TestScene.h"
#include "Engine.h"
#include <iostream>

void TestScene::setup() {

}

void TestScene::init() {

}

void TestScene::start() {

}

void TestScene::update() {
	//std::cout << "test scene" << std::endl;
}

void TestScene::processInput() {
	std::cout << "from test scene input" << std::endl;

	InputManager& inputManager = *Engine::getInstance()->getInputManager();
	if (inputManager.getKeyDown(GLFW_KEY_ESCAPE)) {
		Engine::getInstance()->terminate();
	}
}