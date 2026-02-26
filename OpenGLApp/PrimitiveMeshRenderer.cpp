#include "PrimitiveMeshRenderer.h"
#include "Engine.h"

PrimitiveMeshRenderer::PrimitiveMeshRenderer(): meshPrimitive(nullptr), shader(nullptr) {
	color = glm::vec3(1.0f);
}

void PrimitiveMeshRenderer::render() {
	shader->use();
	Camera* camera = Engine::getInstance()->getCurrentScene()->getCurrentCamera();
	glm::vec3 position = transform->getGlobalPosition();
	glm::vec3 rotation = transform->getGlobalEulerRotation();
	glm::vec3 scale = transform->getGlobalScale();

	glm::mat4 model(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, scale);
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, -1.0f));

	glm::mat4 view = camera->getViewMatrix();

	glm::vec2 screenDimension = Engine::getInstance()->getScreenDimension();
	glm::mat4 projection = 
		glm::perspective(glm::radians(camera->fov), 
		(float)screenDimension.x / (float)screenDimension.y,
		camera->nearPlane, camera->farPlane);

	shader->setMat4("model", model);
	shader->setMat4("view", view);
	shader->setMat4("projection", projection);
	shader->setVec3("color", color);

	meshPrimitive->render();
}