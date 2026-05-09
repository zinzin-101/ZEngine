#include "RenderPipeline.h"
#include "Engine.h"
#include <iostream>

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

void RenderPipeline::addFrameData(unsigned int buffer, std::string name, FrameData::Type type) {
	if (frameData.contains(name)) {
		std::cout << "Frame data with the same name already exists" << std::endl;
		return;
	}

	frameData[name] = FrameData(name, buffer, type);
}