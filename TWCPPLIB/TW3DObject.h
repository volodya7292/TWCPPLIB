#pragma once
#include "TW3DTypes.h"

namespace TW3D {
	struct Transform {
		TWT::Vector3f position = TWT::Vector3f(0);
		TWT::Vector3f rotation = TWT::Vector3f(0);
		TWT::Vector3f scale = TWT::Vector3f(1);

		TWT::Matrix4f GetModelMatrix();
	};

	class TW3DObject {
	public:
		TW3DObject();
		virtual ~TW3DObject();

		Transform transform;
	};
}