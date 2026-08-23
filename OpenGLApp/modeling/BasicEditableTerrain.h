#pragma once
#include "../Component.h"
#include <shader.h>

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
		Shader* shader;
		float horizontalScale;
		float verticalScale;
		glm::vec3 color;

		BasicEditableTerrain(unsigned int width, unsigned int length, float spacing);
		virtual void init() override;
		virtual void update() override;
		virtual void render() override;
};