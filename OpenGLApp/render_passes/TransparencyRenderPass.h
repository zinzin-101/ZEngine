#pragma once
#include "../RenderPass.h"

class TransparencyRenderPass : public RenderPass {
	virtual void render(std::vector<FrameData>& frameData, std::vector<Object*>& objects) override;
};