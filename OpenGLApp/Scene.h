#pragma once
#include <vector>

class Object;

class Scene {
	private:
		std::vector<Object*> objects;

	public:
		Scene();
		virtual ~Scene();
		std::vector<Object*>& getObjects();
		virtual void setup(); // for adding objects
		virtual void init(); // for component setup
		virtual void start(); // for object's custom behavior
		virtual void update();
		void prepareScene();
		virtual void processInput();
};