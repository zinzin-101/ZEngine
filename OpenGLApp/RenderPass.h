#pragma once
#include <string>
#include <vector>

struct FrameBuffer {
	unsigned int fbo;
	std::string name;
	FrameBuffer(std::string name, unsigned int fbo);
};

class RenderPass {
	protected:
		std::vector<FrameBuffer>* frameBuffers;

	public:
		RenderPass(std::vector<FrameBuffer>* frameBuffers);
		virtual void render() = 0;

		void addFrameBuffer(unsigned int fbo, std::string name);
};
