#pragma once
#include "Transform.h"

namespace ComponentConfig {
	constexpr int DEFAULT_PRIORITY = 1;
}

class Component {
	protected:
		int priority;
		Transform* transform;

		bool isTransparent;

	public:
		bool isActive;

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
};

bool compareComponentPriority(const Component* const c1, const Component* const c2);