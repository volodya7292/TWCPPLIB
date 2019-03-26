#pragma once
#include "TW3DTypes.h"

namespace TW3D {
	class TW3DTransform {
	public:
		TW3DTransform(TWT::Vector3f Position = TWT::Vector3f(0), TWT::Vector3f Rotation = TWT::Vector3f(0), TWT::Vector3f Scale = TWT::Vector3f(1), TWT::Bool ChangedInit = true);
		~TW3DTransform() = default;

		TWT::Matrix4f GetModelMatrix();
		TWT::Vector3f GetPosition();
		TWT::Vector3f GetRotation();
		TWT::Vector3f GetScale();
		void SetPosition(const TWT::Vector3f& Position);
		void SetRotation(const TWT::Vector3f& Rotation);
		void SetScale(const TWT::Vector3f& Scale);
		void AdjustRotation(const TWT::Vector3f& Rotation);

		TWT::Bool Changed = false;

	private:
		TWT::Vector3f position = TWT::Vector3f(0);
		TWT::Vector3f rotation = TWT::Vector3f(0);
		TWT::Vector3f scale = TWT::Vector3f(1);
	};
}