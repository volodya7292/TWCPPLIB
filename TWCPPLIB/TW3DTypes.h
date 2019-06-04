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
	using mat4d = glm::mat<4, 4, double>;

	const static DXGI_FORMAT RGBA8Unorm = DXGI_FORMAT_R8G8B8A8_UNORM;
	const static DXGI_FORMAT RGBA16Float = DXGI_FORMAT_R16G16B16A16_FLOAT;
	const static DXGI_FORMAT R32Float = DXGI_FORMAT_R32_FLOAT;
	const static DXGI_FORMAT RGBA32Float = DXGI_FORMAT_R32G32B32A32_FLOAT;

	struct DefaultVertex {
		DefaultVertex(float x, float y, float z, float u, float v, float nx, float ny, float nz) :
			Pos(x, y, z), TexCoord(u, v, 0), Normal(nx, ny, nz) {}

		vec3 Pos;
		vec3 TexCoord; // .z - material ID
		vec3 Normal;
		vec3 Tangent;
		vec3 Bitangent;
	};

	struct DefaultCameraCB {
		vec4 pos;
		mat4 proj;
		mat4 view;
		mat4 proj_view;
		vec4 info = vec4(1.0f, 1.0f, 0, 0); // .x - FOVy in radians, .y - scale factor (large objects are scaled down)
	};

	struct DefaultModelCB {
		mat4 model;
		mat4 model_reduced;
	};

	struct DefaultRendererInfoCB {
		vec4u info; // .x - frame index
	};

	struct Bounds {
		vec4 pMin;
		vec4 pMax;
	};

	template<typename T>
	inline T Min(T V0, T V1) {
		return glm::min(V0, V1);
	}

	template<typename T>
	inline T Max(T V0, T V1) {
		return glm::max(V0, V1);
	}

	template<typename T>
	T Radians(T Degrees) {
		return glm::radians(Degrees);
	}

	template<uint L, typename T>
	T Length(const glm::vec<L, T>& V) {
		return glm::length(V);
	}

	template<uint L>
	glm::vec<L, float> Distance(const glm::vec<L, float>& V0, const glm::vec<L, float>& V1) {
		return glm::distance(V0, V1);
	}

	template<uint L, typename T>
	glm::vec<L, T> Normalize(const glm::vec<L, T>& V) {
		return glm::normalize(V);
	}

	template<uint L>
	inline glm::vec<L, float> Cross(const glm::vec<L, float>& V0, const glm::vec<L, float>& V1) {
		return glm::cross(V0, V1);
	}

	template<typename T>
	inline glm::mat<4, 4, T> Translate(glm::mat<4, 4, T> const& Matrix, glm::vec<3, T> const& Position) {
		return glm::translate(Matrix, Position);
	}

	template<typename T>
	inline glm::mat<4, 4, T> Rotate(glm::mat<4, 4, T> const& Matrix, T Angle, glm::vec<3, T> const& Axis) {
		return glm::rotate(Matrix, Angle, Axis);
	}

	template<typename T>
	inline glm::mat<4, 4, T> Scale(glm::mat<4, 4, T> const& Matrix, glm::vec<3, T> const& Scale) {
		return glm::scale(Matrix, Scale);
	}

	template<typename T>
	inline glm::mat<4, 4, T> Perspective(T Fovy, T Aspect, T ZNear, T ZFar) {
		return glm::perspective(Fovy, Aspect, ZNear, ZFar);
	}
}  // namespace TWT