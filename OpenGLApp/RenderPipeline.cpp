#include "RenderPipeline.h"
#include "Engine.h"
#include <iostream>

RenderPipeline::RenderPipeline(): hasInit(false) {}

RenderPipeline::~RenderPipeline() {
	for (RenderPass* pass : renderPasses) {
		delete pass;
	}
	renderPasses.clear();

	for (const std::pair<std::string, FrameData>& nameToFrameData : frameData) {
		FrameData data = nameToFrameData.second;
		FrameData::Type type = data.type;
		unsigned int buffer = data.buffer;
		switch (type) {
			case FrameData::Type::TEXTURE:
				glDeleteTextures(1, &buffer);
				break;

			case FrameData::Type::FRAME_BUFFER:
				glDeleteFramebuffers(1, &buffer);
				break;

			case FrameData::Type::RENDER_BUFFER:
				glDeleteRenderbuffers(1, &buffer);
				break;

			case FrameData::Type::VAO:
				glDeleteVertexArrays(1, &buffer);
				break;
			}
	}
}

void RenderPipeline::init() {}

void RenderPipeline::initPipeline() {
	init();
	hasInit = true;
}

void RenderPipeline::render(std::vector<Object*>& objects) {
	for (RenderPass* renderPass : renderPasses) {
		renderPass->render(frameData, shaders, objects);
	}
	Engine::getInstance()->getRenderer()->clearTransparencyQueue();
}

void RenderPipeline::addRenderPass(RenderPass* renderPass) {
	renderPasses.emplace_back(renderPass);
}

void RenderPipeline::addShader(Shader* shader, std::string name) {
	if (shaders.contains(name)) {
		std::cout << "shader with the same name already exists" << std::endl;
		return;
	}

	shaders[name] = shader;
}

void RenderPipeline::addFrameData(unsigned int buffer, std::string name, FrameData::Type type, bool shouldSelfClean) {
	if (frameData.contains(name)) {
		std::cout << "Frame data with the same name already exists" << std::endl;
		return;
	}

	frameData[name] = FrameData(name, buffer, type, shouldSelfClean);
}

void RenderPipeline::addFrameData(glm::mat4 matrix, std::string name) {
	if (frameData.contains(name)) {
		std::cout << "Frame data with the same name already exists" << std::endl;
		return;
	}

	frameData[name] = FrameData(name, matrix);
}

bool RenderPipeline::getHasInit() const {
	return hasInit;
}