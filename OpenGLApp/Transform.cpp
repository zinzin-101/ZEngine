#include "Transform.h"
#include "Object.h"

Transform::Transform(Object* owner): object(owner), parent(nullptr) {};

Object* Transform::getObject() {
	return object;
}