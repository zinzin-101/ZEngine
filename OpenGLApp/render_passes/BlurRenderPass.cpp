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

	unsigned int pingpongFBO10 = frameData.at("pingpongFBO10").buffer;
	unsigned int pingpongFBO11 = frameData.at("pingpongFBO11").buffer;
	unsigned int pingpongFBOs1[2] = { pingpongFBO10, pingpongFBO11 };

	unsigned int pingpongColorBuffers10 = frameData.at("pingpongColorBuffers10").buffer;
	unsigned int pingpongColorBuffers11 = frameData.at("pingpongColorBuffers11").buffer;
	unsigned int pingpongColorBuffers1[2] = { pingpongColorBuffers10, pingpongColorBuffers11 };

	unsigned int pingpongFBO20 = frameData.at("pingpongFBO20").buffer;
	unsigned int pingpongFBO21 = frameData.at("pingpongFBO21").buffer;
	unsigned int pingpongFBOs2[2] = { pingpongFBO20, pingpongFBO21 };

	unsigned int pingpongColorBuffers20 = frameData.at("pingpongColorBuffers20").buffer;
	unsigned int pingpongColorBuffers21 = frameData.at("pingpongColorBuffers21").buffer;
	unsigned int pingpongColorBuffers2[2] = { pingpongColorBuffers20, pingpongColorBuffers21 };

	unsigned int sceneColorBuffer0 = frameData.at("sceneColorBuffers0").buffer;
	unsigned int sceneColorBuffer1 = frameData.at("sceneColorBuffers1").buffer;
	unsigned int sceneColorBuffer2 = frameData.at("sceneColorBuffers2").buffer;
	unsigned int sceneColorBuffer3 = frameData.at("sceneColorBuffers3").buffer;
	unsigned int sceneColorBuffers[4] = { sceneColorBuffer0, sceneColorBuffer1, sceneColorBuffer2, sceneColorBuffer3 };
	////
	unsigned int combineFBO = frameData.at("combineFBO").buffer;
	unsigned int combineBuffers0 = frameData.at("combineBuffers0").buffer; // background
	unsigned int combineBuffers1 = frameData.at("combineBuffers1").buffer; // foreground
	unsigned int combineBuffers[2] = { combineBuffers0, combineBuffers1 };
	Shader& blurCombineShader = *shaders.at("blurCombineShader");

	glBindFramebuffer(GL_FRAMEBUFFER, combineFBO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneColorBuffer0);
	blurCombineShader.use();
	blurCombineShader.setInt("image", 0);
	blurCombineShader.setInt("backgroundBlur", 1);
	blurCombineShader.setInt("foregroundBlur", 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneColorBuffer0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sceneColorBuffer1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, sceneColorBuffer2);
	PBRRenderPipeline::renderQuadFromVAO(frameData.at("quadVAO").buffer);

	// blend background
	blurShader.use();
	glDisable(GL_BLEND);
	for (unsigned int i = 0; i < amount; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBOs1[horizontal]);
		blurShader.setInt("horizontal", horizontal);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, isFirstIteration ? combineBuffers0 : pingpongColorBuffers1[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
		PBRRenderPipeline::renderQuadFromVAO(frameData.at("quadVAO").buffer);
		horizontal = !horizontal;
		if (isFirstIteration) {
			isFirstIteration = false;
		}
	}
	unsigned int blurredBackground = pingpongColorBuffers1[!horizontal];

	// blend foreground
	horizontal = true;
	isFirstIteration = true;
	for (unsigned int i = 0; i < amount; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBOs2[horizontal]);
		blurShader.setInt("horizontal", horizontal);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, isFirstIteration ? combineBuffers1 : pingpongColorBuffers2[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
		PBRRenderPipeline::renderQuadFromVAO(frameData.at("quadVAO").buffer);
		horizontal = !horizontal;
		if (isFirstIteration) {
			isFirstIteration = false;
		}
	}
	unsigned int blurredForeground = pingpongColorBuffers2[!horizontal];

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// combine everything
	Shader& blurFinalShader = *shaders.at("blurFinalShader");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	blurFinalShader.use();
	blurFinalShader.setInt("background", 0);
	blurFinalShader.setInt("foreground", 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blurredBackground);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, blurredForeground);
	PBRRenderPipeline::renderQuadFromVAO(frameData.at("quadVAO").buffer);

	glEnable(GL_BLEND);
}