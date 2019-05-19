#pragma once
#include "TWTypes.h"
#include "TW3DTypes.h"
#include "TW3DResourceManager.h"

class TW3DPerspectiveCamera {
public:
	TW3DPerspectiveCamera(TW3DResourceManager* ResourceManager = nullptr, TWT::uint Width = 1280, TWT::uint Height = 720, float FOVY = 70.0f, TWT::vec3 Position = TWT::vec3(0),
		TWT::vec3 Rotation = TWT::vec3(0), float ZNear = 0.1f, float ZFar = 1000.0f);
	~TW3DPerspectiveCamera();

	TWT::mat4 GetProjectionMatrix();
	TWT::mat4 GetViewMatrix();
	TWT::mat4 GetProjectionViewMatrix();
	TWT::vec3 GetRotation();
	float GetRatioX();
	float GetRatioY();

	void SetRotation(TWT::vec3 Rotation);
	void Move(float to_back, float left_right);
	void UpdateConstantBuffer(TW3DConstantBuffer* ConstantBuffer = nullptr);
	TW3DConstantBuffer* GetConstantBuffer();

	TWT::uint Width, Height;
	float FOVY, ZNear, ZFar;
	TWT::vec3 Position;

private:
	TWT::vec3 rotation;
	TW3DConstantBuffer* constant_buffer = nullptr;
};