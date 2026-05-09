#pragma once
#include "RenderPass.h"
#include "Object.h"

class RenderPipeline {
	protected:
		std::vector<RenderPass*> renderPasses;
		std::vector<FrameData> frameData;

		void addRenderPass(RenderPass* renderPass);

	public:
		RenderPipeline();
		~RenderPipeline();
		virtual void init() = 0; // set render passes in individual pipeline class
		void render(std::vector<Object*>& objects);
};
