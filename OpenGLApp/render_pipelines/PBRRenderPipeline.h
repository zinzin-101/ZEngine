#pragma once
#include "../RenderPipeline.h"
#include <shader.h>

class PBRRenderPipeline : public RenderPipeline {
	private:
        Shader pbrShader;
        Shader equirectangularToCubemapShader;
        Shader irradianceShader;
        Shader prefilterShader;
        Shader brdfShader;
        Shader backgroundShader;
        Shader depthShader;
        Shader debugDepthQuad;
        Shader blurShader;
        Shader blurFinalShader;

        unsigned int cubeVAO;
        unsigned int cubeVBO;
        void renderCube();

        unsigned int quadVAO;
        unsigned int quadVBO;
        void renderQuad();

        std::string envMapPath;

        bool useDepthOfField;
        float depthPercentage;

        float backgroundRotateAngle;

    protected:
		virtual void init() override;

	public:

        PBRRenderPipeline();
        ~PBRRenderPipeline();

        void setEnvironmentMap(std::string path);
        void setUseDepthOfField(bool value);
        void setDepthPercentage(float value);
        void setBackgroundRotateAngle(float value);

        bool isUsingDepthOfField() const;
        float getDepthPercentage() const;
        float getBackgroundRotateAngle() const;

        static void renderCubeFromVAO(unsigned int vao);
        static void renderQuadFromVAO(unsigned int vao);
};