#include "RenderPass.h"
#include <cassert>
#include <glad/glad.h>

FrameData::FrameData() : buffer(0), matrix(1.0f), name(""), type(Type::NONE) {}

FrameData::FrameData(std::string name, unsigned int buffer, FrameData::Type type):
	buffer(buffer), matrix(1.0f), name(name), type(type) 
{}

FrameData::FrameData(std::string name, glm::mat4 matrix): buffer(0), matrix(matrix), name(name), type(FrameData::Type::MAT4) {}