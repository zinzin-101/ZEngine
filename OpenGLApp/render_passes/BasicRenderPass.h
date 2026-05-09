#pragma once
#include "../RenderPass.h"

class BasicRenderPass : public RenderPass {
	virtual void render(std::vector<FrameData>& frameData, std::vector<Object*>& objects) override;
};