#pragma once
#include "../RenderPass.h"

class ShadowRenderPass : public RenderPass {
	private:
		void renderObjects(Shader& shader, std::vector<Object*>& objects);
		void renderTransparentObjects(Shader& shader, std::vector<Object*>& objects);

	public:
		virtual void render(std::map<std::string, FrameData>& frameData, std::map<std::string, Shader*> shaders, std::vector<Object*>& objects) override;
};