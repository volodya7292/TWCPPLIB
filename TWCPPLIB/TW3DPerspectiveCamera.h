#pragma once
#include "TWTypes.h"
#include "TW3DTypes.h"
#include "TW3DResourceManager.h"

namespace TW3D {
	class TW3DPerspectiveCamera {
	public:
		TW3DPerspectiveCamera(TW3DResourceManager* ResourceManager = nullptr, TWT::Float FOVY = 70.0f, TWT::Vector3f Position = TWT::Vector3f(0),
			TWT::Vector3f Rotation = TWT::Vector3f(0),TWT::Float ZNear = 0.1f, TWT::Float ZFar = 1000.0f);
		~TW3DPerspectiveCamera();

		TWT::Matrix4f GetProjectionMatrix(TWT::UInt Width, TWT::UInt Height);
		TWT::Matrix4f GetViewMatrix();
		TWT::Matrix4f GetProjectionViewMatrix(TWT::UInt Width, TWT::UInt Height);
		TWT::Vector3f GetRotation();

		void SetRotation(TWT::Vector3f Rotation);

		void UpdateConstantBuffer(TWT::UInt Width, TWT::UInt Height, TW3DResourceCB* ConstantBuffer = nullptr);
		void Use(TW3DGraphicsCommandList* CommandList);

		TWT::Float FOVY, ZNear, ZFar;
		TWT::Vector3f Position;

	private:
		TWT::Vector3f rotation;
		TW3DResourceCB* constant_buffer = nullptr;
	};
}