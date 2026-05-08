#pragma once
#include <string>
#include <vector>

struct FrameBuffer {
	unsigned int fbo;
	std::string name;
	FrameBuffer(std::string name, unsigned int fbo);
};

class RenderPass {
	public:
		virtual void render(std::vector<FrameBuffer>& frameBuffers) = 0;
		static void addFrameBuffer(unsigned int fbo, std::string name, std::vector<FrameBuffer>& frameBuffers);
};
