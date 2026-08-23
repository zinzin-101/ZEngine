#pragma once
#include "../Scene.h"

class BasicTerrainScene: public Scene {
	private:
		Object* terrainObj;

	protected:
		virtual void loadMeshData() override;
		virtual void setup() override; // for adding objects

	public:
		virtual void processInput() override;
};