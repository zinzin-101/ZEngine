#include "BlurRenderPass.h"
#include "../render_pipelines/PBRRenderPipeline.h"
#include "../Engine.h"
#include "../Object.h"
#include <glad/glad.h>

using namespace RendererOperation;

void BlurRenderPass::render(std::map<std::string, FrameData>& frameData, std::map<std::string, Shader*>& shaders, std::vector<Object*>& objects) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	bool useDepthOfField = (bool)frameData.at("useDepthOfField").buffer;
	if (!useDepthOfField) return;

	// blur with two-pass Gaussian blur
	Shader& blurShader = *shaders.at("blurShader");
	bool horizontal = true;
	bool isFirstIteration = true;
	unsigned int amount = 10;
	blurShader.use();

	unsigned int pingpongFBO0 = frameData.at("pingpongFBO0").buffer;
	unsigned int pingpongFBO1 = frameData.at("pingpongFBO1").buffer;
	unsigned int pingpongFBOs[2] = { pingpongFBO0, pingpongFBO1 };

	unsigned int pingpongColorBuffers0 = frameData.at("pingpongColorBuffers0").buffer;
	unsigned int pingpongColorBuffers1 = frameData.at("pingpongColorBuffers1").buffer;
	unsigned int pingpongColorBuffers[2] = { pingpongColorBuffers0, pingpongColorBuffers1 };

	unsigned int sceneColorBuffer0 = frameData.at("sceneColorBuffers0").buffer;
	unsigned int sceneColorBuffer1 = frameData.at("sceneColorBuffers1").buffer;
	unsigned int sceneColorBuffers[2] = {sceneColorBuffer0, sceneColorBuffer1};

	glDisable(GL_BLEND);
	for (unsigned int i = 0; i < amount; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBOs[horizontal]);
		blurShader.setInt("horizontal", horizontal);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, isFirstIteration ? sceneColorBuffers[1] : pingpongColorBuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
		PBRRenderPipeline::renderQuadFromVAO(frameData.at("quadVAO").buffer);
		horizontal = !horizontal;
		if (isFirstIteration) {
			isFirstIteration = false;
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Shader& blurFinalShader = *shaders.at("blurFinalShader");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	blurFinalShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneColorBuffers[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[!horizontal]);
	PBRRenderPipeline::renderQuadFromVAO(frameData.at("quadVAO").buffer);

	glEnable(GL_BLEND);
}