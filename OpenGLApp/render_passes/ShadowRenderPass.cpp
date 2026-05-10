#include "ShadowRenderPass.h"
#include "../Engine.h"
#include "../Object.h"
#include "../Model.h"
#include <glad/glad.h>

using namespace RendererOperation;

void ShadowRenderPass::render(std::map<std::string, FrameData>& frameData, std::map<std::string, Shader*>& shaders, std::vector<Object*>& objects) {
	glm::vec3 lightPosition = glm::vec3(2.5f, 0.5f, 2.0f); // change later
    const unsigned int SHADOW_WIDTH = 4096;
    const unsigned int SHADOW_HEIGHT = 4096;
	/////////////////////////////////////////////////

    // render scene depth to texture from light source
    glm::mat4 lightProjection;
    glm::mat4 lightView;
    glm::mat4 lightSpaceMatrix;
    float nearPlane = 2.0f;
    float farPlane = 12.0f;
    lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, nearPlane, farPlane);
    lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;
    Shader& depthShader = *shaders.at("depthShader");
    depthShader.use();
    depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, frameData.at("depthMapFBO").buffer);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    //glCullFace(GL_FRONT);

    for (Object* object : objects) {
        Model* objectModel = object->getFirstComponentOfType<Model>();
        if (objectModel != nullptr) {
            glm::mat4 modelMat = object->transform.getGlobalModelMatrix();
            depthShader.setMat4("model", modelMat);
            objectModel->drawGeometry();
        }
    }

    renderObjects(depthShader, objects);
    renderTransparentObjects(depthShader, objects);

    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // reset viewport
    glm::vec2 screenDimension = Engine::getInstance()->getScreenDimension();
    glViewport(0, 0, (int)screenDimension.x, (int)screenDimension.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    frameData["lightSpaceMatrix"] = FrameData("lightSpaceMatrix", lightSpaceMatrix);
}

void ShadowRenderPass::renderObjects(Shader& shader, std::vector<Object*>& objects) {
    for (Object* object : objects) {
        shader.setMat4("model", object->transform.getGlobalModelMatrix());
        object->render();
    }
}

void ShadowRenderPass::renderTransparentObjects(Shader& shader, std::vector<Object*>& objects) {
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
        shader.setMat4("model", object->transform.getGlobalModelMatrix());
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