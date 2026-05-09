#include "RenderPass.h"
#include <cassert>
#include <glad/glad.h>

FrameData::FrameData() : buffer(0), name(""), type(Type::NONE) {}

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