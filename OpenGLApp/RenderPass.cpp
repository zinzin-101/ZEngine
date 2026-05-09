#include "RenderPass.h"
#include <cassert>
#include <glad/glad.h>

FrameData::FrameData(std::string name, unsigned int buffer, FrameData::Type type): buffer(buffer), name(name), type(type) {}

FrameData::~FrameData() {
	switch (type) {
		case Type::TEXTURE:
			glDeleteTextures(1, &buffer);
			break;

		case Type::FRAME_BUFFER:
			glDeleteFramebuffers(1, &buffer);
			break;
	}
}

void RenderPass::addFrameBuffer(unsigned int buffer, std::string name, FrameData::Type type, std::vector<FrameData>& frameData) {
	frameData.emplace_back(name, buffer, type);
}