#pragma once

namespace ComponentConfig {
	constexpr int DEFAULT_PRIORITY = -1;
}

class Component {
	private:
		int priority;

	public:
		Component();
		int getComponentPriority() const;
		virtual ~Component();
		virtual void start() = 0;
		virtual void update() = 0;
};

bool compareComponentPriority(const Component* const c1, const Component* const c2);