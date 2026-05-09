#pragma once
#include <string>
#include <vector>
#include <map>

class Object;

struct FrameData {
	enum Type {
		TEXTURE,
		FRAME_BUFFER,
		NONE
	};

	unsigned int buffer;
	std::string name;
	Type type;

	FrameData();
	FrameData(std::string name, unsigned int buffer, Type type);
	~FrameData();
};

class RenderPass {
	public:
		virtual void render(std::map<std::string, FrameData>& frameData, std::vector<Object*>& objects) = 0;
};
