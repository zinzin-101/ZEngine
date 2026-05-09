#pragma once
#include "../RenderPass.h"

class BasicRenderPass : public RenderPass {
	virtual void render(std::map<std::string, FrameData>& frameData, std::map<std::string, Shader*> shaders, std::vector<Object*>& objects) override;
};