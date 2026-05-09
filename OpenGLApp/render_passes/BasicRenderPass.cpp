#include "BasicRenderPass.h"
#include "../Engine.h"
#include "../Object.h"
#include <glad/glad.h>

using namespace RendererOperation;

void BasicRenderPass::render(std::map<std::string, FrameData>& frameData, std::map<std::string, Shader*> shaders, std::vector<Object*>& objects) {
	for (Object* object : objects) {
		object->render();
	}
}