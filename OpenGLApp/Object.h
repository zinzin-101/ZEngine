#pragma once
#include "Component.h"
#include "Transform.h"
#include <algorithm>
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
		bool isActive;

		virtual void init(); // for initializing component 
		virtual void start(); // for custom behavior
		virtual void update(); // for updating component
		virtual void render(); // for component that can render
		void setCurrentScene(Scene* scene);
		void addComponent(Component* component);

		template <class T>
		T* addComponent();

		template <class T>
		T* addComponent(std::string arg);

		template <class T>
		T* getFirstComponentOfType();

		void removeComponent(Component* component);
		bool getCanDelete() const;

		static void deleteObject(Object* object);
};

template <class T>
T* Object::addComponent() {
	T* componentType = new T();
	Component* component = dynamic_cast<Component*>(componentType);

	assert(component != nullptr && "Trying to add a non-component type");
	component->setTransform(&transform);
	components.emplace_back(component);
	std::sort(components.begin(), components.end(), compareComponentPriority);

	return componentType;
}

template <class T>
T* Object::addComponent(std::string arg) {
	T* componentType = new T(arg);
	Component* component = dynamic_cast<Component*>(componentType);

	assert(component != nullptr && "Trying to add a non-component type");
	component->setTransform(&transform);
	components.emplace_back(component);
	std::sort(components.begin(), components.end(), compareComponentPriority);

	return componentType;
}

template <class T>
T* Object::getFirstComponentOfType() {
	for (Component* component : components) {
		T* targetComponent = dynamic_cast<T*>(component);
		if (targetComponent != nullptr) {
			return targetComponent;
		}
	}

	return nullptr;
}