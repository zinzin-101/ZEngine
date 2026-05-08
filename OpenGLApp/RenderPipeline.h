#pragma once
#include "RenderPass.h"

class RenderPipeline {
	private:
		std::vector<RenderPass> renderPasses;
		std::vector<FrameBuffer> frameBuffers;

	public:
		RenderPipeline();
		virtual void init() = 0; // set render passes in individual pipeline class
		void render();
};
