#include "RenderPipeline.h"

RenderPipeline::RenderPipeline(){
	init();
}

void RenderPipeline::render() {
	for (RenderPass& renderPass : renderPasses) {
		renderPass.render(frameBuffers);
	}
}