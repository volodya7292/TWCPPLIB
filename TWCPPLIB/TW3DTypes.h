#pragma once
#include "TWTypes.h"

namespace TWT {
	using Vector2f = glm::vec2;
	using Vector3f = glm::vec3;
	using Vector4f = glm::vec4;
	using Matrix4f = glm::mat4;

	struct DefaultVertex {
		DefaultVertex(TWT::Float x, TWT::Float y, TWT::Float z, TWT::Float u, TWT::Float v) : Pos(x, y, z), TexCoord(u, v) {}
		TWT::Vector3f Pos;
		TWT::Vector2f TexCoord;
	};

	struct DefaultCameraCB {
		TWT::Matrix4f proj_view;
	};

	struct DefaultPerObjectCB {
		TWT::Matrix4f model;
	};

	inline Matrix4f Translate(Matrix4f Matrix, TWT::Vector3f Position) {
		return glm::translate(Matrix, Position);
	}

	inline Matrix4f Rotate(Matrix4f Matrix, TWT::Float Angle, TWT::Vector3f Axis) {
		return glm::rotate(Matrix, Angle, Axis);
	}
}