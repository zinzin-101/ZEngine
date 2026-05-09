#include "RenderPipeline.h"
#include "Engine.h"

RenderPipeline::RenderPipeline() {}

RenderPipeline::~RenderPipeline() {
	for (RenderPass* pass : renderPasses) {
		delete pass;
	}
	renderPasses.clear();
}

void RenderPipeline::render(std::vector<Object*>& objects) {
	for (RenderPass* renderPass : renderPasses) {
		renderPass->render(frameData, objects);
	}
	Engine::getInstance()->getRenderer()->clearTransparencyQueue();
}

void RenderPipeline::addRenderPass(RenderPass* renderPass) {
	renderPasses.emplace_back(renderPass);
}