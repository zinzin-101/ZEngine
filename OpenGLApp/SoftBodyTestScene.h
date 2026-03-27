#pragma once
#include "Scene.h"
#include "TetrahedronSoftBodyMesh.h"

class SoftBodyTestScene : public Scene {
	private:
		SoftBodyMesh* softbodymesh;
		int dragIndex;
		float lastInvMass;
		float dragDepth;

	protected:
		virtual void loadMeshData() override;
		virtual void setup() override; // for adding objects

	public:
		virtual void processInput() override;
};