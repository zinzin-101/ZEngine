#pragma once
#include "Scene.h"

class Model;

class PBRScene : public Scene {
	private:
		Object* root;
		float depthPercentage;
		Model* modelPtr;

	protected:
		virtual void loadMeshData() override;
		virtual void setup() override;

	public:
		virtual ~PBRScene() override;
		virtual void processInput() override;
};