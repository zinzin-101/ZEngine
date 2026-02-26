#pragma once
#include "Scene.h"

class TestScene : public Scene {
	public:
		virtual void loadMeshData() override;
		virtual void setup() override; // for adding objects
		virtual void processInput() override;
};