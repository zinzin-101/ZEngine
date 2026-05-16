#pragma once
#include "Transform.h"

namespace ComponentConfig {
	constexpr int DEFAULT_PRIORITY = 1;
}

class Shader;

class Component {
	protected:
		int priority;
		Transform* transform;

		bool isTransparent;

	public:
		bool isActive;
		bool autoDeleteOnDestroy;

		Component();
		virtual ~Component();
		void setTransform(Transform* transform);
		Transform* getTransform();
		int getComponentPriority() const;
		bool getIsTransparent() const;
		virtual void init();
		virtual void start();
		virtual void preUpdate();
		virtual void update();
		virtual void postUpdate();
		virtual void render();
		virtual void render(Shader* shader);
};

bool compareComponentPriority(const Component* const c1, const Component* const c2);