#pragma once
#include "TWTypes.h"

namespace TWT {
	using Vector2f = glm::vec2;
	using Vector2u = glm::uvec2;
	using Vector3f = glm::vec3;
	using Vector4f = glm::vec4;
	using Matrix4f = glm::mat4;

	
	const static DXGI_FORMAT RGBA8Unorm = DXGI_FORMAT_R8G8B8A8_UNORM;
	const static DXGI_FORMAT RGBA16Float = DXGI_FORMAT_R16G16B16A16_FLOAT;
	const static DXGI_FORMAT RGBA32Float = DXGI_FORMAT_R32G32B32A32_FLOAT;
	const static DXGI_FORMAT D24UnormS8 = DXGI_FORMAT_D24_UNORM_S8_UINT;

	struct DefaultVertex {
		DefaultVertex(TWT::Float x, TWT::Float y, TWT::Float z, TWT::Float u, TWT::Float v, TWT::Float nx, TWT::Float ny, TWT::Float nz) :
			Pos(x, y, z), TexCoord(u, v), Normal(nx, ny, nz) {}

		TWT::Vector3f Pos;
		TWT::Vector2f TexCoord;
		TWT::Vector3f Normal;
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