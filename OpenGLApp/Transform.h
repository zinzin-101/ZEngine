#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class Object;

class Transform {
	private:
		Object* object;
		Transform* parent;
		std::vector<Transform*> children;

		void removeSelfFromParent(Transform* parent);

	public:
		Transform(Object* owner);
		Object* getObject();
		glm::vec3 getGlobalPosition() const;
		glm::vec3 getGlobalEulerRotation() const;
		glm::vec3 getGlobalScale() const;
		void setParent(Transform* parent);
		glm::vec3 position;
		glm::vec3 eulerRotation;
		glm::vec3 scale;
};