#pragma once
#include "SceneManager.h"
#include "Renderer.h"

namespace EngineConfig {
	constexpr unsigned int DEFAULT_SCREEN_WIDTH = 1600;
	constexpr unsigned int DEFAULT_SCREEN_HEIGHT = 900;
	constexpr bool INIT_IN_FULL_SCREEN = false;
	inline const char* WINDOW_NAME = "ZEngine";
	constexpr SceneManagerConfig::SceneId INITIAL_SCENE_ID = SceneManagerConfig::PBR_SCENE;
	constexpr float FIXED_DT = 1.0f / 144.0f;
	constexpr RendererOperation::RenderPipelines INITIAL_RENDERING_PIPELINE = RendererOperation::RenderPipelines::PBR;
}