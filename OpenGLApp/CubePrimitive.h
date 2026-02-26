#pragma once
#include "MeshPrimitive.h"

class CubePrimitive : public MeshPrimitive {
	public:
		CubePrimitive();
		virtual void init() override;
		virtual void render() override;
};