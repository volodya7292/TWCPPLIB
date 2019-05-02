#pragma once
#include "TWTypes.h"
#include "TW3DTypes.h"
#include "TW3DResourceManager.h"

namespace TW3D {
	class TW3DPerspectiveCamera {
	public:
		TW3DPerspectiveCamera(TW3DResourceManager* ResourceManager = nullptr, TWT::UInt Width = 1280, TWT::UInt Height = 720, TWT::Float FOVY = 70.0f, TWT::Vector3f Position = TWT::Vector3f(0),
			TWT::Vector3f Rotation = TWT::Vector3f(0),TWT::Float ZNear = 0.1f, TWT::Float ZFar = 1000.0f);
		~TW3DPerspectiveCamera();

		TWT::Matrix4f GetProjectionMatrix();
		TWT::Matrix4f GetViewMatrix();
		TWT::Matrix4f GetProjectionViewMatrix();
		TWT::Vector3f GetRotation();
		TWT::Float GetRatioX();
		TWT::Float GetRatioY();

		void SetRotation(TWT::Vector3f Rotation);
		void Move(TWT::Float to_back, TWT::Float left_right);
		void UpdateConstantBuffer(TW3DResourceCB* ConstantBuffer = nullptr);
		TW3DResourceCB* GetConstantBuffer();

		TWT::UInt Width, Height;
		TWT::Float FOVY, ZNear, ZFar;
		TWT::Vector3f Position;

	private:
		TWT::Vector3f rotation;
		TW3DResourceCB* constant_buffer = nullptr;
	};
}