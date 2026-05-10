#pragma once
#include <map>
#include <queue>
#include <string>
#include <shader.h>
#include "MeshPrimitive.h"
#include "RenderPipeline.h"

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

	enum RenderPipelines {
		SIMPLE,
		PBR
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

		RenderPipeline* currentRenderPipeline;

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
		void clearTransparencyQueue();
		std::priority_queue<
			RendererOperation::TransparentComponentRendering,
			std::vector<RendererOperation::TransparentComponentRendering>,
			RendererOperation::TransparencyComparator
		>& getTransparencyQueue();

		void setCurrentRenderPipeline(RenderPipeline* pipeline);
		RenderPipeline* getCurrentRenderPipeline();

		void clear();

		void setViewPort(int x, int y, int width, int height);

		static RenderPipeline* getRenderPipeline(RendererOperation::RenderPipelines pipeline);
};