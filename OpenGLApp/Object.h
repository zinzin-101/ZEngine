#pragma once
#include "Component.h"
#include "Transform.h"
#include <algorithm>
#include <vector>
#include <string>

class Scene;
class Shader;

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
		virtual void preUpdate(); // for updating component
		virtual void update(); // for updating component
		virtual void postUpdate(); // for updating component
		virtual void render(bool isRenderingTransparent = false); // for component that can render
		virtual void render(Shader* shader);
		void setCurrentScene(Scene* scene);
		void addComponent(Component* component);

		template <class T>
		T* addComponent();

		template <class T, class... Args>
		T* addComponent(Args... args);

		template <class T>
		T* getFirstComponentOfType();

		void removeComponent(Component* component);
		bool getCanDelete() const;

		Scene* getCurrentScene();

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

template <class T, class... Args>
T* Object::addComponent(Args... args) {
	T* componentType = new T(args...);
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