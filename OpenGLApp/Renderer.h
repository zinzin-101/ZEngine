#pragma once
#include <map>
#include <queue>
#include <string>
#include <shader.h>
#include "MeshPrimitive.h"

class Shader;
class Component;

namespace RendererOperation {
	struct TransparentComponentRendering {
		Component* component;
		float distanceToCamera;
		TransparentComponentRendering(Component* component, float distanceToCamera);
	};

	struct TransparencyComparator {
		bool operator()(const TransparentComponentRendering& c1, const TransparentComponentRendering& c2);
	};
}


class Renderer {
	private:
		std::map<std::string, Shader*> nameToShader;
		std::map<std::string, MeshPrimitive*> nameToMeshPrimitive;
		void clearMesh();
		void clearShader();
		
		std::priority_queue<
			RendererOperation::TransparentComponentRendering,
			std::vector<RendererOperation::TransparentComponentRendering>,
			RendererOperation::TransparencyComparator
		> transparencyRenderQueue;

	public:
		Renderer();
		~Renderer();
		bool init();
		void render();

		void addMesh(std::string name, MeshPrimitive* meshPrimitive);
		MeshPrimitive* getMesh(std::string name);

		void addShader(std::string name, Shader* shader);
		Shader* getShader(std::string name);

		void addToTransparencyQueue(Component* component);

		void clear();

		void setViewPort(int x, int y, int width, int height);
};