#include "RenderPass.h"
#include <cassert>
#include <glad/glad.h>

FrameBuffer::FrameBuffer(std::string name, unsigned int fbo): fbo(fbo), name(name) {}

void RenderPass::addFrameBuffer(unsigned int fbo, std::string name, std::vector<FrameBuffer>& frameBuffers) {
	frameBuffers.emplace_back(name, fbo);
}