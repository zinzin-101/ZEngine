#pragma once
#include "Component.h"
#include "shader.h"
#include "MeshPrimitive.h"

class PrimitiveMeshRenderer : public Component {
	public:
		MeshPrimitive* meshPrimitive;
		Shader* shader;
		glm::vec3 color;
		PrimitiveMeshRenderer();
		virtual void render() override;
};