#pragma once
#include "RenderPass.h"
#include "Object.h"

class RenderPipeline {
	private:
		bool hasInit;

	protected:
		std::vector<RenderPass*> renderPasses;
		std::map<std::string, FrameData> frameData;
		std::map<std::string, Shader*> shaders;

		void addRenderPass(RenderPass* renderPass);
		void addShader(Shader* shader, std::string name);
		void addFrameData(unsigned int buffer, std::string name, FrameData::Type type, bool shouldSelfClean = true);
		void addFrameData(glm::mat4 matrix, std::string name);

		virtual void init(); // set render passes in individual pipeline class

	public:
		RenderPipeline();
		~RenderPipeline();
		void initPipeline();
		void render(std::vector<Object*>& objects);

		bool getHasInit() const;
};
