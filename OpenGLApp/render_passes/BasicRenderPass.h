#pragma once
#include "../RenderPass.h"

class BasicRenderPass : public RenderPass {
	virtual void render(std::map<std::string, FrameData>& frameData, std::vector<Object*>& objects) override;
};