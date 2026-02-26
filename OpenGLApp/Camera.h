#pragma once
#include "Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace CameraConfig {
	constexpr float DEFAULT_FOV = 60.0f;
}

class Camera : public Component {
	private:
		glm::vec3 forward;
		glm::vec3 right;
		glm::vec3 up;
		glm::vec3 worldUp;

	public:
		float fov;
		Camera(Transform* transform);
		void setForward(glm::vec3 forward);
		glm::mat4 getViewMatrix() const;

		virtual void update() override;
};