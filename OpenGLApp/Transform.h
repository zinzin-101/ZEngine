#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Object;

class Transform {
	private:
		Object* object;

	public:
		Transform(Object* owner);
		Object* getObject();
		glm::vec3 getGlobalPosition() const;
		glm::vec3 getGlobalEulerRotation() const;
		glm::vec3 getGlobalScale() const;
		Transform* parent;
		glm::vec3 position;
		glm::vec3 eulerRotation;
		glm::vec3 scale;
};