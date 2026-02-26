#pragma once
#include <map>
#include <string>
#include <shader.h>
#include "MeshPrimitive.h"

class Shader;

class Renderer {
	private:
		std::map<std::string, Shader*> nameToShader;
		std::map<std::string, MeshPrimitive*> nameToMeshPrimitive;
		void clearMesh();
		void clearShader();

	public:
		Renderer();
		~Renderer();
		bool init();
		void render();

		void addMesh(std::string name, MeshPrimitive* meshPrimitive);
		MeshPrimitive* getMesh(std::string name);

		void addShader(std::string name, Shader* shader);
		Shader* getShader(std::string name);

		void clear();

		void setViewPort(int x, int y, int width, int height);
};