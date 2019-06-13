#pragma once
#include "TWTypes.h"
#include "TW3DTypes.h"
#include "TW3DResourceManager.h"

class TW3DPerspectiveCamera {
public:
	TW3DPerspectiveCamera(TW3DResourceManager* ResourceManager, TWT::uint Width = 1280, TWT::uint Height = 720, float FOVY = 70.0f, TWT::float3 Position = TWT::float3(0),
		TWT::float3 Rotation = TWT::float3(0), float ZNear = 0.1f, float ZFar = 1000.0f);
	~TW3DPerspectiveCamera();

	TWT::float4x4 GetProjectionMatrix();
	TWT::float4x4 GetViewMatrix();
	TWT::float4x4 GetProjectionViewMatrix();
	TWT::float3 GetRotation();
	float GetRatioX();
	float GetRatioY();

	void SetRotation(TWT::float3 Rotation);
	void Move(float to_back, float left_right);
	void UpdateConstantBuffer();
	TW3DConstantBuffer* GetConstantBuffer();
	TW3DConstantBuffer* GetPreviousConstantBuffer();

	TWT::uint Width, Height;
	float FOVY, ZNear, ZFar;
	TWT::float3 Position;

private:
	TWT::float3 rotation;
	TW3DConstantBuffer* constant_buffer = nullptr;
	TW3DConstantBuffer* prev_constant_buffer = nullptr;
	TWT::DefaultCameraCB cb;
};