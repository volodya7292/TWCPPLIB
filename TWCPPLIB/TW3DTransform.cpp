#include "pch.h"
#include "TW3DTransform.h"

TW3DTransform::TW3DTransform(TWT::double3 Position, TWT::double3 Rotation, TWT::double3 Scale, bool UpdatedInit) :
	position(Position), rotation(Rotation), scale(Scale), Updated(UpdatedInit) {
}

TWT::double4x4 TW3DTransform::GetModelMatrix() const {
	TWT::double4x4 m = TWT::Translate(TWT::double4x4(1), position);
	m = TWT::Rotate(m, TWT::Radians(rotation.x), TWT::double3(1, 0, 0));
	m = TWT::Rotate(m, TWT::Radians(rotation.y), TWT::double3(0, 1, 0));
	m = TWT::Rotate(m, TWT::Radians(rotation.z), TWT::double3(0, 0, 1));
	return TWT::Scale(m, scale);
}

TWT::float4x4 TW3DTransform::GetModelMatrixReduced(TWT::double3 const& RelativePoint, double ScaleFactor) const {
	TWT::double3 to_point = position - RelativePoint;
	TWT::double3 pos_new = RelativePoint + TWT::Normalize(to_point) * (TWT::Length(to_point) * ScaleFactor);
	TWT::double4x4 model_new = TWT::Scale(GetModelMatrix(), TWT::double3(ScaleFactor));
	return TWT::Translate(model_new, pos_new);
}

TWT::double3 TW3DTransform::GetPosition() const {
	return position;
}

TWT::double3 TW3DTransform::GetRotation() const {
	return rotation;
}

TWT::double3 TW3DTransform::GetScale() const {
	return scale;
}

void TW3DTransform::SetIdentity() {
	position = TWT::double3(0);
	rotation = TWT::double3(0);
	scale = TWT::double3(1);
	Updated = true;

}

void TW3DTransform::SetPosition(TWT::double3 const& Position) {
	position = Position;
	Updated = true;
}

void TW3DTransform::SetRotation(TWT::double3 const& Rotation) {
	rotation = TWT::float3(fmod(Rotation.x, 360.0f), fmod(Rotation.y, 360.0f), fmod(Rotation.z, 360.0f));
	Updated = true;
}

void TW3DTransform::SetScale(TWT::double3 const& Scale) {
	scale = Scale;
	Updated = true;
}

void TW3DTransform::AdjustRotation(TWT::double3 const& Rotation) {
	SetRotation(rotation + Rotation);
}

rp3d::Transform TW3DTransform::GetPhysicalTransform() {
	TWT::float3 rotrad = TWT::Radians(rotation);
	rp3d::Vector3 initPosition(position.x, position.y, position.z);
	rp3d::Quaternion initOrientation = rp3d::Quaternion::fromEulerAngles(rp3d::Vector3(rotrad.x, rotrad.y, rotrad.z));

	return rp3d::Transform(initPosition, initOrientation);
}

void TW3DTransform::InitFromPhysicalTransform(rp3d::Transform const& Transform) {
	rp3d::Vector3 initPosition = Transform.getPosition();
	position = TWT::float3(initPosition.x, initPosition.y, initPosition.z);
	rp3d::Quaternion initOrientation = Transform.getOrientation();

	glm::quat quater = glm::quat(initOrientation.x, initOrientation.y, initOrientation.z, initOrientation.w);
	
	rotation = TWT::Degrees(eulerAngles(quater));
	rotation.z = fmod(180 - rotation.z, 360.0f);
}
