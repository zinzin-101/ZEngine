#include "TransparencyRenderPass.h"
#include "../Engine.h"
#include "../Object.h"
#include <glad/glad.h>

using namespace RendererOperation;

void TransparencyRenderPass::render(std::map<std::string, FrameData>& frameData, std::map<std::string, Shader*>& shaders, std::vector<Object*>& objects) {
	//bool useDepthOfField = (bool)frameData.at("useDepthOfField").buffer;
	//if (useDepthOfField) {
	//	unsigned int sceneFBO = frameData.at("sceneFBO").buffer;
	//	glBindFramebuffer(GL_READ_FRAMEBUFFER, sceneFBO);
	//	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	//	// Copy depth buffer to screen
	//	glm::vec2 screenDimension = Engine::getInstance()->getScreenDimension();
	//	int width = (int)screenDimension.x;
	//	int height = (int)screenDimension.y;
	//	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	//}

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// save previous GL states
	GLboolean powerWasEnabled;
	GLint prevSrcRGB, prevDstRGB, prevSrcAlpha, prevDstAlpha;
	GLboolean prevDepthMask;

	powerWasEnabled = glIsEnabled(GL_BLEND);
	glGetIntegerv(GL_BLEND_SRC_RGB, &prevSrcRGB);
	glGetIntegerv(GL_BLEND_DST_RGB, &prevDstRGB);
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &prevSrcAlpha);
	glGetIntegerv(GL_BLEND_DST_ALPHA, &prevDstAlpha);
	glGetBooleanv(GL_DEPTH_WRITEMASK, &prevDepthMask);

	// apply GL states for transparency rendering
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	for (Object* object : objects) {
		object->render(true);
	}

	auto& transparencyRenderQueue = Engine::getInstance()->getRenderer()->getTransparencyQueue();

	while (!transparencyRenderQueue.empty()) {
		TransparentComponentRendering transparentComponent = transparencyRenderQueue.top();
		transparencyRenderQueue.pop();
		transparentComponent.component->render();
	}

	// restore previous GL states
	powerWasEnabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
	glBlendFuncSeparate(prevSrcRGB, prevDstRGB, prevSrcAlpha, prevDstAlpha);
	glDepthMask(prevDepthMask);
}