#include "RenderPass.h"
#include <cassert>
#include <glad/glad.h>

FrameBuffer::FrameBuffer(std::string name, unsigned int fbo): fbo(fbo), name(name) {}
	
RenderPass::RenderPass(std::vector<FrameBuffer>* frameBuffers): frameBuffers(frameBuffers) {
	assert(frameBuffers != nullptr && "Frame buffer vector is null");
};

void RenderPass::addFrameBuffer(unsigned int fbo, std::string name) {
	frameBuffers->emplace_back(name, fbo);
}