#include "SimpleRenderPipeline.h"
#include "../render_passes/BasicRenderPass.h"
#include "../render_passes/TransparencyRenderPass.h"

void SimpleRenderPipeline::init() {
	addRenderPass(new BasicRenderPass());
	addRenderPass(new TransparencyRenderPass());
}