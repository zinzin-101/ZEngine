#pragma once
#include "../SmokeSim.h"

class SmokeMover : public Component {
	private:
		SmokeSim* sim;
		float t;
		glm::vec3 origin;

	public:
		float angularSpeed;
		float radius;

		SmokeMover();
		virtual void start() override;
		virtual void update() override;
};