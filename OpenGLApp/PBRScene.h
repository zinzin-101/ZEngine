#pragma once
#include "Scene.h"

class PBRScene : public Scene {
	protected:
		virtual void loadMeshData() override;
		virtual void setup() override;

	public:
		virtual void processInput() override;
};