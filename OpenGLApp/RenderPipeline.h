#pragma once
#include "RenderPass.h"
#include "Object.h"

class RenderPipeline {
	protected:
		std::vector<RenderPass*> renderPasses;
		std::map<std::string, FrameData> frameData;
		std::map<std::string, Shader*> shaders;

		void addRenderPass(RenderPass* renderPass);
		void addShader(Shader* shader, std::string name);
		void addFrameData(unsigned int buffer, std::string name, FrameData::Type type);

	public:
		RenderPipeline();
		~RenderPipeline();
		virtual void init() = 0; // set render passes in individual pipeline class
		void render(std::vector<Object*>& objects);
};
