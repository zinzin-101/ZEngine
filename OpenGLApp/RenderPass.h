#pragma once
#include "Object.h"
#include <string>
#include <vector>
#include <map>

struct FrameData {
	enum Type {
		TEXTURE,
		FRAME_BUFFER
	};

	unsigned int buffer;
	std::string name;
	Type type;

	FrameData(std::string name, unsigned int buffer, Type type);
	~FrameData();
};

class RenderPass {
	public:
		virtual void render(std::map<FrameData>& frameData, std::vector<Object*>& objects) = 0;
};
