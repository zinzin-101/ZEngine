#pragma once
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Ray {
	private:
		glm::vec3 origin;
		glm::vec3 direction;
		float distance;

	public:
		Ray(glm::vec3 origin, glm::vec3 direction, float distance);
		float getDistanceFromPoint(glm::vec3 point);
		static glm::vec3 getRayDirectionFromScreen(glm::vec2 mousePosition, Camera* camera);
};