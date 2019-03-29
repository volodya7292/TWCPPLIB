#include "pch.h"
#include "TW3DPerspectiveCamera.h"

TW3D::TW3DPerspectiveCamera::TW3DPerspectiveCamera(TW3DResourceManager* ResourceManager, TWT::Float FOVY, TWT::Vector3f Position, TWT::Vector3f Rotation, TWT::Float ZNear, TWT::Float ZFar) :
	FOVY(FOVY), Position(Position), rotation(Rotation), ZNear(ZNear), ZFar(ZFar)
{
	if (ResourceManager)
		constant_buffer = ResourceManager->CreateConstantBuffer(sizeof(TWT::DefaultCameraCB));
}

TW3D::TW3DPerspectiveCamera::~TW3DPerspectiveCamera() {
	if (constant_buffer)
		delete constant_buffer;
}

TWT::Matrix4f TW3D::TW3DPerspectiveCamera::GetProjectionMatrix(TWT::UInt Width, TWT::UInt Height) {
	TWT::Matrix4f f = glm::perspective(glm::radians(FOVY), (float)Width / Height, ZNear, ZFar);
	return f;
}

TWT::Matrix4f TW3D::TW3DPerspectiveCamera::GetViewMatrix() {
	TWT::Matrix4f m = TWT::Rotate(TWT::Matrix4f(1), glm::radians(rotation.x), TWT::Vector3f(1, 0, 0));
	m *= TWT::Rotate(TWT::Matrix4f(1), glm::radians(rotation.y), TWT::Vector3f(0, 1, 0));
	m *= TWT::Rotate(TWT::Matrix4f(1), glm::radians(rotation.z), TWT::Vector3f(0, 0, 1));
	return m * TWT::Translate(TWT::Matrix4f(1), -Position);
}

TWT::Matrix4f TW3D::TW3DPerspectiveCamera::GetProjectionViewMatrix(TWT::UInt Width, TWT::UInt Height) {
	return GetProjectionMatrix(Width, Height) * GetViewMatrix();
}

TWT::Vector3f TW3D::TW3DPerspectiveCamera::GetRotation() {
	return rotation;
}

void TW3D::TW3DPerspectiveCamera::SetRotation(TWT::Vector3f Rotation) {
	rotation = glm::mod(Rotation, 360.0f);
}

void TW3D::TW3DPerspectiveCamera::UpdateConstantBuffer(TWT::UInt Width, TWT::UInt Height, TW3DResourceCB* ConstantBuffer) {
	TWT::DefaultCameraCB cb;
	cb.proj_view = GetProjectionViewMatrix(Width, Height);

	if (ConstantBuffer)
		ConstantBuffer->Update(&cb, 0);
	else
		constant_buffer->Update(&cb, 0);
}

void TW3D::TW3DPerspectiveCamera::Use(TW3DGraphicsCommandList* CommandList) {
	CommandList->SetGraphicsRootCBV(0, constant_buffer, 0);
}
