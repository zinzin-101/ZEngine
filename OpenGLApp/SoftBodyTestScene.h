#pragma once
#include "Scene.h"
#include "TetrahedronSoftBodyMesh.h"

class SoftBodyTestScene : public Scene {
	private:
		TetrahedronSoftBodyMesh* softbodymesh;

	public:
		virtual void loadMeshData() override;
		virtual void setup() override; // for adding objects
		virtual void processInput() override;
};