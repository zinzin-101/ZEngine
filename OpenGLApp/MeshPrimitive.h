#pragma once
#include "Component.h"

class MeshPrimitive {
	protected:
		unsigned int vao;
		unsigned int vbo;
		unsigned int ebo;

	public:
		MeshPrimitive();
		~MeshPrimitive();
		virtual void init() = 0;
		virtual void render() = 0;
};