#pragma once
#include "Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace CameraConfig {
	constexpr float DEFAULT_FOV = 60.0f;
	constexpr float DEFAULT_NEAR_PLANE = 0.01f;
	constexpr float DEFAULT_FAR_PLANE = 100.0f;
}

class Camera : public Component {
	private:
		glm::vec3 forward;
		glm::vec3 right;
		glm::vec3 up;
		glm::vec3 worldUp;

		void updateCameraVector();

	public:
		float fov;
		float nearPlane;
		float farPlane;
		Camera();
		void setForward(glm::vec3 forward);
		glm::mat4 getViewMatrix() const;
		glm::vec3 getFoward() const;
		glm::vec3 getRight() const;
		glm::vec3 getUp() const;

		virtual void init() override;
		virtual void update() override;
};