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

        void loadIBL();

    protected:
		virtual void init() override;

	public:
        PBRRenderPipeline();
        ~PBRRenderPipeline();

        void setEnvironmentMap(std::string path);
        void setUseDepthOfField(bool value);

        bool isUsingDepthOfField() const;

        static void renderCubeFromVAO(unsigned int vao);
        static void renderQuadFromVAO(unsigned int vao);
};