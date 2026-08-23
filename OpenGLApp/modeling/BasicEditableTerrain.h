#pragma once
#include "../Component.h"
#include "../ComputeShader.h"
#include <shader.h>

class BasicEditableTerrain : public Component {
	private:
		unsigned int width;
		unsigned int length;
		float spacing;

		unsigned int terrainStripCount;
		unsigned int vertsPerTerrainStrip;

		unsigned int ssbo;
		unsigned int vao;
		unsigned int vbo;
		unsigned int ebo;

		unsigned int vertsCount;
		ComputeShader computeShader;

	public:
		Shader* shader;
		float horizontalScale;
		float verticalScale;
		glm::vec3 color;
		float radius;
		bool isGrowing;
		float growAmount;

		BasicEditableTerrain(unsigned int width, unsigned int length, float spacing);
		~BasicEditableTerrain();
		virtual void init() override;
		virtual void update() override;
		virtual void render() override;
};