#pragma once
#include "Scene.h"

class TestScene : public Scene {
	protected:
		virtual void loadMeshData() override;
		virtual void setup() override; // for adding objects

	public:
		virtual void processInput() override;
};