#include "pch.h"
#include "TW3DPerspectiveCamera.h"

TW3DPerspectiveCamera::TW3DPerspectiveCamera(TW3DResourceManager* ResourceManager, TWT::uint Width, TWT::uint Height,
	float FOVY, TWT::float3 Position, TWT::float3 Rotation, float ZNear, float ZFar) :
	Width(Width), Height(Height), FOVY(FOVY), Position(Position), rotation(Rotation), ZNear(ZNear), ZFar(ZFar)
{
	if (ResourceManager)
		constant_buffer = ResourceManager->CreateConstantBuffer(1, sizeof(TWT::DefaultCameraCB));
}

TW3DPerspectiveCamera::~TW3DPerspectiveCamera() {
	delete constant_buffer;
}

TWT::float4x4 TW3DPerspectiveCamera::GetProjectionMatrix() {
	return TWT::Perspective(TWT::Radians(FOVY), static_cast<float>(Width) / Height, ZNear, ZFar);
}

TWT::float4x4 TW3DPerspectiveCamera::GetViewMatrix() {
	TWT::float4x4 m = TWT::Rotate(TWT::float4x4(1), TWT::Radians(rotation.x), TWT::float3(1, 0, 0));
	m *= TWT::Rotate(TWT::float4x4(1), TWT::Radians(rotation.y), TWT::float3(0, 1, 0));
	m *= TWT::Rotate(TWT::float4x4(1), TWT::Radians(rotation.z), TWT::float3(0, 0, 1));
	return m * TWT::Translate(TWT::float4x4(1), -Position);
}

TWT::float4x4 TW3DPerspectiveCamera::GetProjectionViewMatrix() {
	return GetProjectionMatrix() * GetViewMatrix();
}

TWT::float3 TW3DPerspectiveCamera::GetRotation() {
	return rotation;
}

float TW3DPerspectiveCamera::GetRatioX() {
	return tan(TWT::Radians(FOVY) / 2.0f) * (static_cast<float>(Width) / Height);
}

float TW3DPerspectiveCamera::GetRatioY() {
	return tan(TWT::Radians(FOVY) / 2.0f);
}

void TW3DPerspectiveCamera::SetRotation(TWT::float3 Rotation) {
	rotation = TWT::float3(fmod(Rotation.x, 360.0f), fmod(Rotation.y, 360.0f), fmod(Rotation.z, 360.0f));
}

void TW3DPerspectiveCamera::Move(float to_back, float left_right) {
	TWT::float3 d = TWT::float3(sin(TWT::Radians(-rotation.y)), 0, cos(TWT::Radians(-rotation.y)));
	Position -= d * to_back;
	Position -= TWT::Normalize(TWT::Cross(d, TWT::float3(0, 1, 0))) * left_right;
}

void TW3DPerspectiveCamera::UpdateConstantBuffer(TW3DConstantBuffer* ConstantBuffer) {
	TWT::DefaultCameraCB cb;
	cb.pos = TWT::float4(Position, 1);
	cb.proj = GetProjectionMatrix();
	cb.view = GetViewMatrix();
	cb.proj_view = GetProjectionViewMatrix();
	cb.info.x = TWT::Radians(FOVY);

	if (ConstantBuffer)
		ConstantBuffer->Update(&cb, 0);
	else
		constant_buffer->Update(&cb, 0);
}

TW3DConstantBuffer* TW3DPerspectiveCamera::GetConstantBuffer() {
	return constant_buffer;
}
