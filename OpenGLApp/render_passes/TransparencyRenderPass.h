#pragma once
#include "../RenderPass.h"

class TransparencyRenderPass : public RenderPass {
	virtual void render(std::map<std::string, FrameData>& frameData, std::vector<Object*>& objects) override;
};