#include "PBRRenderPass.h"
#include "../render_pipelines/PBRRenderPipeline.h"
#include "../Engine.h"
#include "../Object.h"
#include "../Model.h"
#include <glad/glad.h>

using namespace RendererOperation;

void PBRRenderPass::render(std::map<std::string, FrameData>& frameData, std::map<std::string, Shader*>& shaders, std::vector<Object*>& objects) {

	Shader& pbrShader = *shaders.at("pbrShader");
    pbrShader.use();
    pbrShader.setInt("numOfLights", 0);
    pbrShader.setVec3("lightPositions[0]", glm::vec3(2.5f, 0.5f, 2.0f));
    pbrShader.setVec3("lightPositions[1]", glm::vec3(2.5f, 0.5f, 2.0f));
    pbrShader.setVec3("lightPositions[2]", glm::vec3(2.5f, 0.5f, 2.0f));
    pbrShader.setVec3("lightPositions[3]", glm::vec3(2.5f, 0.5f, 2.0f));
    pbrShader.setVec3("lightColors[0]", glm::vec3(2000.0f, 2000.0f, 2000.0f));
    pbrShader.setVec3("lightColors[1]", glm::vec3(2000.0f, 2000.0f, 2000.0f));
    pbrShader.setVec3("lightColors[2]", glm::vec3(2000.0f, 2000.0f, 2000.0f));
    pbrShader.setVec3("lightColors[3]", glm::vec3(2000.0f, 2000.0f, 2000.0f));

    Camera& camera = *Engine::getInstance()->getCurrentScene()->getCurrentCamera();

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.getViewMatrix();
    pbrShader.setMat4("view", view);
    pbrShader.setVec3("camPos", camera.getTransform()->getGlobalPosition());
    pbrShader.setBool("useDiffuseShadow", true);
    pbrShader.setBool("useCartoonShading", false);

    unsigned int irradianceMap = frameData.at("irradianceMap").buffer;
    unsigned int prefilterMap = frameData.at("prefilterMap").buffer;
    unsigned int brdfLUTTexture = frameData.at("brdfLUTTexture").buffer;
    unsigned int depthMap = frameData.at("depthMap").buffer;

    // bind pre-computed IBL data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

    // bind shadow map
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    bool useDepthOfField = (bool)frameData.at("useDepthOfField").buffer;

    // bind frame buffer
    if (useDepthOfField) {
        unsigned int sceneFBO = frameData.at("sceneFBO").buffer;
        glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    glm::mat4 lightSpaceMatrix = frameData.at("lightSpaceMatrix").matrix;
    //pbrShader.setMat4("envMapRotation", glm::mat4(1.0f));
    pbrShader.setMat4("envMapRotation", frameData.at("envMapRotation").matrix);
    pbrShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    pbrShader.setBool("useDepthOfField", useDepthOfField);
    pbrShader.setFloat("farPlane", camera.farPlane);
    pbrShader.setFloat("nearPlane", camera.nearPlane);
    for (Object* object : objects) {
        Model* objectModel = object->getFirstComponentOfType<Model>();
        if (objectModel != nullptr) {
            glm::mat4 modelMat = object->transform.getGlobalModelMatrix();
            pbrShader.setMat4("model", modelMat);
            pbrShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(modelMat))));
            objectModel->draw(pbrShader);
        }
    }

    // render skybox
    Shader& backgroundShader = *shaders.at("backgroundShader");
    unsigned int envCubemap = frameData.at("envCubemap").buffer;

    backgroundShader.use();
    backgroundShader.setBool("useDepthOfField", useDepthOfField);
    backgroundShader.setFloat("farPlane", camera.farPlane);
    backgroundShader.setFloat("nearPlane", camera.nearPlane);
    backgroundShader.setMat4("view", view);
    model = glm::mat4(1.0f);
    backgroundShader.setMat4("model", frameData.at("envMapRotation").matrix * model);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glDepthFunc(GL_LEQUAL);
    PBRRenderPipeline::renderCubeFromVAO(frameData.at("cubeVAO").buffer);
    glDepthFunc(GL_LESS);
}