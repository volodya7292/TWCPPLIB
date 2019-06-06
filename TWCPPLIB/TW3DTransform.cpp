#include "pch.h"
#include "TW3DTransform.h"

TW3DTransform::TW3DTransform(TWT::vec3d Position, TWT::vec3d Rotation, TWT::vec3d Scale, bool UpdatedInit) :
	position(Position), rotation(Rotation), scale(Scale), Updated(UpdatedInit) {
}

TWT::mat4d TW3DTransform::GetModelMatrix() const {
	TWT::mat4d m = TWT::Translate(TWT::mat4d(1), position);
	m = TWT::Rotate(m, TWT::Radians(rotation.x), TWT::vec3d(1, 0, 0));
	m = TWT::Rotate(m, TWT::Radians(rotation.y), TWT::vec3d(0, 1, 0));
	m = TWT::Rotate(m, TWT::Radians(rotation.z), TWT::vec3d(0, 0, 1));
	return TWT::Scale(m, scale);
}

TWT::mat4 TW3DTransform::GetModelMatrixReduced(TWT::vec3d const& RelativePoint, double ScaleFactor) const {
	TWT::vec3d to_point = position - RelativePoint;
	TWT::vec3d pos_new = RelativePoint + TWT::Normalize(to_point) * (TWT::Length(to_point) * ScaleFactor);
	TWT::mat4d model_new = TWT::Scale(GetModelMatrix(), TWT::vec3d(ScaleFactor));
	return TWT::Translate(model_new, pos_new);
}

TWT::vec3d TW3DTransform::GetPosition() const {
	return position;
}

TWT::vec3d TW3DTransform::GetRotation() const {
	return rotation;
}

TWT::vec3d TW3DTransform::GetScale() const {
	return scale;
}

void TW3DTransform::SetIdentity() {
	position = TWT::vec3d(0);
	rotation = TWT::vec3d(0);
	scale = TWT::vec3d(1);
	Updated = true;

}

void TW3DTransform::SetPosition(TWT::vec3d const& Position) {
	position = Position;
	Updated = true;
}

void TW3DTransform::SetRotation(TWT::vec3d const& Rotation) {
	rotation = TWT::vec3(fmod(Rotation.x, 360.0f), fmod(Rotation.y, 360.0f), fmod(Rotation.z, 360.0f));
	Updated = true;
}

void TW3DTransform::SetScale(TWT::vec3d const& Scale) {
	scale = Scale;
	Updated = true;
}

void TW3DTransform::AdjustRotation(TWT::vec3d const& Rotation) {
	SetRotation(rotation + Rotation);
}

rp3d::Transform TW3DTransform::GetPhysicalTransform() {
	TWT::vec3 rotrad = TWT::Radians(rotation);
	rp3d::Vector3 initPosition(position.x, position.y, position.z);
	rp3d::Quaternion initOrientation = rp3d::Quaternion::fromEulerAngles(rp3d::Vector3(rotrad.x, rotrad.y, rotrad.z));

	return rp3d::Transform(initPosition, initOrientation);
}
#include "TW3DUtils.h"
void TW3DTransform::InitFromPhysicalTransform(rp3d::Transform const& Transform) {
	rp3d::Vector3 initPosition = Transform.getPosition();
	position = TWT::vec3(initPosition.x, initPosition.y, initPosition.z);
	rp3d::Quaternion initOrientation = Transform.getOrientation();

	glm::quat quater = glm::quat(initOrientation.x, initOrientation.y, initOrientation.z, initOrientation.w);
	
	rotation = TWT::Degrees(glm::eulerAngles(quater));
	rotation.z = fmod(180 - rotation.z, 360.0f);
}
