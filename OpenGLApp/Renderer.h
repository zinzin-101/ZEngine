#pragma once
#include <vector>

class Shader;

class Renderer {
	private:
		std::vector<Shader*> shaders;
	public:
		Renderer();
		~Renderer();
		bool init();
		int addShader(Shader* shader);
		Shader* getShader(int id);

		void setViewPort(int x, int y, int width, int height);
};