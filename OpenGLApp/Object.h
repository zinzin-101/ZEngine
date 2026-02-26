#pragma once
#include "Component.h"
#include "Transform.h"
#include <vector>
#include <string>

class Scene;

class Object {
	private: 
		Scene* currentScene;
		std::vector<Component*> components;
		bool canDelete;

	public:
		Object();
		Object(std::string name);
		virtual ~Object();
		Transform transform;
		std::string name;

		virtual void init(); // for initializing component 
		virtual void start(); // for custom behavior
		virtual void update(); // for updating component
		virtual void render(); // for component that can render
		void setCurrentScene(Scene* scene);
		void addComponent(Component* component);
		void removeComponent(Component* component);
		bool getCanDelete() const;

		static void deleteObject(Object* object);
};