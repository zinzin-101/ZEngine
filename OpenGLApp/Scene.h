#pragma once
#include "Camera.h"
#include <glm/glm.hpp>
#include <vector>

class Object;

class Scene {
	protected:
		std::vector<Object*> objects;
		Camera* currentCamera;

	public:
		Scene();
		virtual ~Scene();
		std::vector<Object*>& getObjects();
		virtual void loadMeshData();
		virtual void setup(); // for adding objects
		virtual void init(); // for component setup
		virtual void start(); // for object's custom behavior
		virtual void update();
		virtual void render();
		void prepareScene();
		virtual void processInput();
		Object* instantiateObject(glm::vec3 position);
		Camera* getCurrentCamera();
};