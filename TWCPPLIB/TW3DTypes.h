#pragma once
#include "TWTypes.h"

namespace TWT {
	using float2    = glm::vec<2, float>;
	using double2   = glm::vec<2, double>;
	using int2      = glm::vec<2, int>;
	using uint2     = glm::vec<2, TWT::uint>;
	using float3    = glm::vec<3, float>;
	using double3   = glm::vec<3, double>;
	using uint3     = glm::vec<3, TWT::uint>;
	using float4    = glm::vec<4, float>;
	using uint4     = glm::vec<4, TWT::uint>;
	using float4x4  = glm::mat<4, 4, float>;
	using double4x4 = glm::mat<4, 4, double>;

	const static DXGI_FORMAT R16Float    = DXGI_FORMAT_R16_FLOAT;
	const static DXGI_FORMAT R32UInt     = DXGI_FORMAT_R32_UINT;
	const static DXGI_FORMAT RG16Float   = DXGI_FORMAT_R16G16_FLOAT;
	const static DXGI_FORMAT RG32Float   = DXGI_FORMAT_R32G32_FLOAT;
	const static DXGI_FORMAT RGB32Float  = DXGI_FORMAT_R32G32B32_FLOAT;
	const static DXGI_FORMAT RGBA8Unorm  = DXGI_FORMAT_R8G8B8A8_UNORM;
	const static DXGI_FORMAT RGBA16Float = DXGI_FORMAT_R16G16B16A16_FLOAT;
	const static DXGI_FORMAT RGBA32Float = DXGI_FORMAT_R32G32B32A32_FLOAT;
	const static DXGI_FORMAT RGBA32UInt  = DXGI_FORMAT_R32G32B32A32_UINT;

	struct DefaultVertex {
		DefaultVertex() {
		}

		DefaultVertex(float x, float y, float z, float u, float v, float nx, float ny, float nz) :
			Pos(x, y, z), TexCoord(u, v, 0), Normal(nx, ny, nz) {
		}

		float3 Pos;
		float3 TexCoord; // .z - material ID
		float3 Normal;
		float3 Tangent;
	};

	struct DefaultCameraCB {
		float4 pos;
		float4x4 proj;
		float4x4 view;
		float4x4 proj_view;
		float4 info; // .x - FOVy in radians
	};

	struct DefaultModelCB {
		float4x4 model;
		float4x4 prev_model;
	};

	struct DefaultRendererInfoCB {
		uint4 info; // .x - width, .y - height, .z - frame index, .w - scale factor for large objects
	};

	struct DefaultMaterial {
		uint4 texture_ids0 = uint4(-1); // .x - diffuse, .y - specular, .z - normal
		uint4 texture_ids1 = uint4(-1); // .x - diffuse, .y - specular, .z - normal
		// if texture_id == -1 then the parameters below are used
		float4 diffuse;  // .a - opacity/translucency
		float4 specular; // .a - roughness
		float4 emission; // .a - texture lerp blend factor
	};

	struct DefaultLightSource {
		TWT::float4 v[3];      // v[0] - sphere position; v[0], v[1], v[2] - triangle vertices
		TWT::float4 normal;    // .xyz - triangle normal, .w - triangle material id
		TWT::float4 info;      // .x - type, .y - sphere radius
	};

	struct Bounds {
		float4 pMin;
		float4 pMax;
	};

	template<typename T>
	inline float2 AsFloat2(const T *const Ptr) {
		return glm::make_vec2(Ptr);
	}

	template<typename T>
	inline float3 AsFloat3(const T *const Ptr) {
		return glm::make_vec3(Ptr);
	}

	template<typename T>
	T Radians(T Degrees) {
		return glm::radians(Degrees);
	}

	template<uint L>
	glm::vec<L, float> Radians(glm::vec<L, float> const& V) {
		return glm::vec<L, float>(glm::radians(V));
	}

	template<typename T>
	T Degrees(T Radians) {
		return glm::radians(Radians);
	}

	template<uint L>
	glm::vec<L, float> Degrees(glm::vec<L, float> const& V) {
		return glm::vec<L, float>(glm::degrees(V));
	}

	template<uint L, typename T>
	T Length(const glm::vec<L, T>& V) {
		return glm::length(V);
	}

	template<uint L>
	glm::vec<L, float> Distance(glm::vec<L, float> const& V0, glm::vec<L, float> const& V1) {
		return glm::distance(V0, V1);
	}

	template<uint L, typename T>
	glm::vec<L, T> Normalize(glm::vec<L, T> const& V) {
		return glm::normalize(V);
	}

	template<uint L>
	inline glm::vec<L, float> Cross(glm::vec<L, float> const& V0, glm::vec<L, float> const& V1) {
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

	template<typename T>
	inline glm::mat<4, 4, T> InfinitePerspective(T Fovy, T Aspect) {
		assert(abs(Aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));

		T const tanHalfFovy = tan(Fovy / static_cast<T>(2));

		glm::mat<4, 4, T> Result(static_cast<T>(0));
		Result[0][0] = static_cast<T>(1) / (Aspect * tanHalfFovy);
		Result[1][1] = static_cast<T>(1) / (tanHalfFovy);
		Result[2][2] = -1;
		Result[2][3] = -static_cast<T>(1);
		Result[3][2] = static_cast<T>(0);
		return Result;
	}

	template<typename T>
	inline glm::mat<4, 4, T> ViewMatrix(glm::vec<3, T> Position, glm::vec<3, T> Rotation) {
		glm::mat<4, 4, T> m = TWT::Rotate(glm::mat<4, 4, T>(1), TWT::Radians(Rotation.x), glm::vec<3, T>(1, 0, 0));
		m *= TWT::Rotate(glm::mat<4, 4, T>(1), TWT::Radians(Rotation.y), glm::vec<3, T>(0, 1, 0));
		m *= TWT::Rotate(glm::mat<4, 4, T>(1), TWT::Radians(Rotation.z), glm::vec<3, T>(0, 0, 1));
		return m * TWT::Translate(glm::mat<4, 4, T>(1), -Position);
	}
}  // namespace TWT