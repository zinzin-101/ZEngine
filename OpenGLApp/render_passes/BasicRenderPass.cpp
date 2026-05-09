#include "BasicRenderPass.h"
#include "../Engine.h"
#include <glad/glad.h>

using namespace RendererOperation;

void BasicRenderPass::render(std::vector<FrameData>& frameData, std::vector<Object*>& objects) {
	for (Object* object : objects) {
		object->render();
	}
}