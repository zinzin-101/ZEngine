#pragma once
#include "../Scene.h"

class BasicEditableTerrain;

class BasicTerrainScene: public Scene {
	private:
		Object* terrainObj;
		BasicEditableTerrain* terrain;

	protected:
		virtual void loadMeshData() override;
		virtual void setup() override; // for adding objects

	public:
		virtual void processInput() override;
};