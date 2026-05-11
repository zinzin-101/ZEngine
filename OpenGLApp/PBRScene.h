#pragma once
#include "Scene.h"

class PBRScene : public Scene {
	private:
		Object* root;
		float depthPercentage;

	protected:
		virtual void loadMeshData() override;
		virtual void setup() override;

	public:
		virtual void processInput() override;
};