#include "pch.h"
#include "TW3DObject.h"

TWT::Matrix4f TW3D::Transform::GetModelMatrix() {
	TWT::Matrix4f m = glm::translate(TWT::Matrix4f(1), position);
	m = glm::rotate(m, rotation.x, TWT::Vector3f(1, 0, 0));
	m = glm::rotate(m, rotation.y, TWT::Vector3f(0, 1, 0));
	m = glm::rotate(m, rotation.z, TWT::Vector3f(0, 0, 1));
	return glm::scale(m, scale);
}


TW3D::TW3DObject::TW3DObject() {
}

TW3D::TW3DObject::~TW3DObject() {
}