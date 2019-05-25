#pragma once
#include "TWTypes.h"

namespace TWT {
	using vec2 = glm::vec2;
	using vec2d = glm::vec<2, double>;
	using vec2i = glm::ivec2;
	using vec2u = glm::uvec2;
	using vec3 = glm::vec3;
	using vec3d = glm::vec<3, double>;
	using vec3u = glm::uvec3;
	using vec4 = glm::vec4;
	using vec4u = glm::uvec4;
	using mat4 = glm::mat4;

	
	const static DXGI_FORMAT RGBA8Unorm = DXGI_FORMAT_R8G8B8A8_UNORM;
	const static DXGI_FORMAT RGBA16Float = DXGI_FORMAT_R16G16B16A16_FLOAT;
	const static DXGI_FORMAT RGBA32Float = DXGI_FORMAT_R32G32B32A32_FLOAT;
	const static DXGI_FORMAT D24UnormS8 = DXGI_FORMAT_D24_UNORM_S8_UINT;

	struct DefaultVertex {
		DefaultVertex(float x, float y, float z, float u, float v, float nx, float ny, float nz) :
			Pos(x, y, z), TexCoord(u, v), Normal(nx, ny, nz) {}

		TWT::vec3 Pos;
		TWT::vec2 TexCoord;
		TWT::vec3 Normal;
	};

	struct DefaultCameraCB {
		TWT::vec4 pos;
		TWT::mat4 proj;
		TWT::mat4 view;
		TWT::mat4 proj_view;
	};

	struct DefaultModelCB {
		TWT::mat4 model;
	};

	struct Bounds {
		TWT::vec4 pMin;
		TWT::vec4 pMax;
	};

	inline mat4 Translate(mat4 Matrix, TWT::vec3 Position) {
		return glm::translate(Matrix, Position);
	}

	inline mat4 Rotate(mat4 Matrix, float Angle, TWT::vec3 Axis) {
		return glm::rotate(Matrix, Angle, Axis);
	}
}