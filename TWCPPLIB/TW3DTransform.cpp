#include "pch.h"
#include "TW3DTransform.h"

TW3DTransform::TW3DTransform(TWT::Vector3f Position, TWT::Vector3f Rotation, TWT::Vector3f Scale, TWT::Bool ChangedInit) :
	position(Position), rotation(Rotation), scale(Scale), Changed(ChangedInit)
{
}

TWT::Matrix4f TW3DTransform::GetModelMatrix() {
	TWT::Matrix4f m = glm::translate(TWT::Matrix4f(1), position);
	m = glm::rotate(m, rotation.x, TWT::Vector3f(1, 0, 0));
	m = glm::rotate(m, rotation.y, TWT::Vector3f(0, 1, 0));
	m = glm::rotate(m, rotation.z, TWT::Vector3f(0, 0, 1));
	return glm::scale(m, scale);
}

TWT::Vector3f TW3DTransform::GetPosition() {
	return position;
}

TWT::Vector3f TW3DTransform::GetRotation() {
	return rotation;
}

TWT::Vector3f TW3DTransform::GetScale() {
	return scale;
}

void TW3DTransform::SetPosition(const TWT::Vector3f& Position) {
	position = Position;
	Changed = true;
}

void TW3DTransform::SetRotation(const TWT::Vector3f& Rotation) {
	rotation = glm::mod(Rotation, TWT::Vector3f(360));
	Changed = true;
}

void TW3DTransform::SetScale(const TWT::Vector3f& Scale) {
	scale = Scale;
	Changed = true;
}

void TW3DTransform::AdjustRotation(const TWT::Vector3f& Rotation) {
	SetRotation(rotation + Rotation);
}
