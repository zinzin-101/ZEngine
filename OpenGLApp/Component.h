#pragma once
#include "Transform.h"

namespace ComponentConfig {
	constexpr int DEFAULT_PRIORITY = 1;
}

class Component {
	protected:
		int priority;
		Transform* transform;

	public:
		Component(Transform* transform);
		virtual ~Component();
		void setTransform(Transform* transform);
		int getComponentPriority() const;
		virtual void start() = 0;
		virtual void update() = 0;
		virtual void render();
};

bool compareComponentPriority(const Component* const c1, const Component* const c2);