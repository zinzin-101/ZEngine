#pragma once
#include "../Component.h"

class BasicEditableTerrain : public Component {
	private:
		unsigned int width;
		unsigned int length;
		float spacing;

		unsigned int terrainStripCount;
		unsigned int vertsPerTerrainStrip;

		unsigned int vao;
		unsigned int vbo;
		unsigned int ebo;

	public:
		float horizontalScale;
		float verticalScale;

		BasicEditableTerrain(unsigned int width, unsigned int length, float spacing);
		virtual void init() override;
};