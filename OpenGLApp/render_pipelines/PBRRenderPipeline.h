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

        unsigned int cubeVAO;
        unsigned int cubeVBO;
        void renderCube();

        unsigned int quadVAO;
        unsigned int quadVBO;
        void renderQuad();

    protected:
		virtual void init() override;

	public:
        PBRRenderPipeline();
        ~PBRRenderPipeline();

        static void renderCubeFromVAO(unsigned int vao);
        static void renderQuadFromVAO(unsigned int vao);
};