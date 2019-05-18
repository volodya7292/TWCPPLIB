#include "pch.h"
#include "TW3DTransform.h"

TW3DTransform::TW3DTransform(TWT::vec3 Position, TWT::vec3 Rotation, TWT::vec3 Scale, bool ChangedInit) :
	position(Position), rotation(Rotation), scale(Scale), Changed(ChangedInit)
{
}

TWT::mat4 TW3DTransform::GetModelMatrix() {
	TWT::mat4 m = glm::translate(TWT::mat4(1), position);
	m = glm::rotate(m, rotation.x, TWT::vec3(1, 0, 0));
	m = glm::rotate(m, rotation.y, TWT::vec3(0, 1, 0));
	m = glm::rotate(m, rotation.z, TWT::vec3(0, 0, 1));
	return glm::scale(m, scale);
}

TWT::vec3 TW3DTransform::GetPosition() {
	return position;
}

TWT::vec3 TW3DTransform::GetRotation() {
	return rotation;
}

TWT::vec3 TW3DTransform::GetScale() {
	return scale;
}

void TW3DTransform::SetPosition(const TWT::vec3& Position) {
	position = Position;
	Changed = true;
}

void TW3DTransform::SetRotation(const TWT::vec3& Rotation) {
	rotation = glm::mod(Rotation, TWT::vec3(360));
	Changed = true;
}

void TW3DTransform::SetScale(const TWT::vec3& Scale) {
	scale = Scale;
	Changed = true;
}

void TW3DTransform::AdjustRotation(const TWT::vec3& Rotation) {
	SetRotation(rotation + Rotation);
}
