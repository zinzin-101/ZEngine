#pragma once
#include "Camera.h"
#include <glm/glm.hpp>
#include <vector>

class Object;

class Scene {
	private:
		bool hasInitialized;
		virtual void init(); // for component setup
		virtual void start(); // for object's custom behavior

	protected:
		std::vector<Object*> objects;
		Camera* currentCamera;

		virtual void loadMeshData();
		virtual void setup(); // for adding objects

	public:
		Scene();
		virtual ~Scene();
		std::vector<Object*>& getObjects();
		virtual void update();
		virtual void render();
		void prepareScene();
		virtual void processInput();
		Object* instantiateObject(glm::vec3 position);
		Camera* getCurrentCamera();
		bool hasBeenInitialized() const;
};