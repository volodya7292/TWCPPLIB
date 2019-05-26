#include "pch.h"
#include "TW3DTransform.h"

TW3DTransform::TW3DTransform(TWT::vec3d Position, TWT::vec3d Rotation, TWT::vec3d Scale, bool ChangedInit) :
	position(Position), rotation(Rotation), scale(Scale), Changed(ChangedInit)
{
}

TWT::mat4d TW3DTransform::GetModelMatrix() {
	TWT::mat4d m = TWT::Translate(TWT::mat4d(1), position);
	m = TWT::Rotate(m, rotation.x, TWT::vec3d(1, 0, 0));
	m = TWT::Rotate(m, rotation.y, TWT::vec3d(0, 1, 0));
	m = TWT::Rotate(m, rotation.z, TWT::vec3d(0, 0, 1));
	return TWT::Scale(m, scale);
}

TWT::mat4 TW3DTransform::GetModelMatrixReduced(TWT::vec3d const& RelativePoint, double ScaleFactor) {
	TWT::vec3d to_point = position - RelativePoint;
	TWT::vec3d pos_new = RelativePoint + TWT::Normalize(to_point) * (TWT::Length(to_point) * ScaleFactor);
	TWT::mat4d model_new = TWT::Scale(GetModelMatrix(), TWT::vec3d(ScaleFactor));
	return TWT::Translate(model_new, pos_new);
}

TWT::vec3d TW3DTransform::GetPosition() {
	return position;
}

TWT::vec3d TW3DTransform::GetRotation() {
	return rotation;
}

TWT::vec3d TW3DTransform::GetScale() {
	return scale;
}

void TW3DTransform::SetPosition(TWT::vec3d const& Position) {
	position = Position;
	Changed = true;
}

void TW3DTransform::SetRotation(TWT::vec3d const& Rotation) {
	rotation = TWT::vec3(fmod(Rotation.x, 360.0f), fmod(Rotation.y, 360.0f), fmod(Rotation.z, 360.0f));
	Changed = true;
}

void TW3DTransform::SetScale(TWT::vec3d const& Scale) {
	scale = Scale;
	Changed = true;
}

void TW3DTransform::AdjustRotation(TWT::vec3d const& Rotation) {
	SetRotation(rotation + Rotation);
}
