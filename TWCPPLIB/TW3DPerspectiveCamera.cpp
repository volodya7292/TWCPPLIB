#include "pch.h"
#include "TW3DPerspectiveCamera.h"

TW3D::TW3DPerspectiveCamera::TW3DPerspectiveCamera(TW3DResourceManager* ResourceManager, TWT::UInt Width, TWT::UInt Height,
	TWT::Float FOVY, TWT::Vector3f Position, TWT::Vector3f Rotation, TWT::Float ZNear, TWT::Float ZFar) :
	Width(Width), Height(Height), FOVY(FOVY), Position(Position), rotation(Rotation), ZNear(ZNear), ZFar(ZFar)
{
	if (ResourceManager)
		constant_buffer = ResourceManager->CreateConstantBuffer(1, sizeof(TWT::DefaultCameraCB));
}

TW3D::TW3DPerspectiveCamera::~TW3DPerspectiveCamera() {
	if (constant_buffer)
		delete constant_buffer;
}

TWT::Matrix4f TW3D::TW3DPerspectiveCamera::GetProjectionMatrix() {
	return glm::perspective(glm::radians(FOVY), (float)Width / Height, ZNear, ZFar);
}

TWT::Matrix4f TW3D::TW3DPerspectiveCamera::GetViewMatrix() {
	TWT::Matrix4f m = TWT::Rotate(TWT::Matrix4f(1), glm::radians(rotation.x), TWT::Vector3f(1, 0, 0));
	m *= TWT::Rotate(TWT::Matrix4f(1), glm::radians(rotation.y), TWT::Vector3f(0, 1, 0));
	m *= TWT::Rotate(TWT::Matrix4f(1), glm::radians(rotation.z), TWT::Vector3f(0, 0, 1));
	return m * TWT::Translate(TWT::Matrix4f(1), -Position);
}

TWT::Matrix4f TW3D::TW3DPerspectiveCamera::GetProjectionViewMatrix() {
	return GetProjectionMatrix() * GetViewMatrix();
}

TWT::Vector3f TW3D::TW3DPerspectiveCamera::GetRotation() {
	return rotation;
}

TWT::Float TW3D::TW3DPerspectiveCamera::GetRatioX() {
	return tan(glm::radians(FOVY) / 2.0f) * ((float)Width / Height);
}

TWT::Float TW3D::TW3DPerspectiveCamera::GetRatioY() {
	return tan(glm::radians(FOVY) / 2.0f);
}

void TW3D::TW3DPerspectiveCamera::SetRotation(TWT::Vector3f Rotation) {
	rotation = glm::mod(Rotation, 360.0f);
}

void TW3D::TW3DPerspectiveCamera::UpdateConstantBuffer(TW3DResourceCB* ConstantBuffer) {
	TWT::DefaultCameraCB cb;
	cb.pos = TWT::Vector4f(Position, 1);
	cb.proj = GetProjectionMatrix();
	cb.view = GetViewMatrix();
	cb.proj_view = GetProjectionViewMatrix();

	if (ConstantBuffer)
		ConstantBuffer->Update(&cb, 0);
	else
		constant_buffer->Update(&cb, 0);
}

TW3D::TW3DResourceCB* TW3D::TW3DPerspectiveCamera::GetConstantBuffer() {
	return constant_buffer;
}
