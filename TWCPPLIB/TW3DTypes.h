#pragma once
#include "TWTypes.h"

namespace TWT {
	using Vector2f = glm::vec2;
	using Vector2u = glm::uvec2;
	using Vector3f = glm::vec3;
	using Vector4f = glm::vec4;
	using Vector4u = glm::uvec4;
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

	struct DefaultVertexMeshCB {
		TWT::Vector4u vertex_info;
		// .x = vertex_offset
		// .y = vertex_count
	};

	struct DefaultVertexBufferCB {
		TWT::Vector4u vertex_offset;
	};

	struct DefaultVertexMeshInstanceCB {
		TWT::Matrix4f model;
	};

	struct Bounds {
		TWT::Vector3f pMin;
		TWT::Vector3f pMax;

		Bounds() :
			pMin(TWT::Vector3f(0)), pMax(TWT::Vector3f(0)) {
		}

		Bounds(TWT::Vector3f pMin, TWT::Vector3f pMax) :
			pMin(pMin), pMax(pMax) {
		}

		TWT::Vector3f offset(TWT::Vector3f p) {
			TWT::Vector3f o = p - pMin;
			if (pMax.x > pMin.x) o.x /= pMax.x - pMin.x;
			if (pMax.y > pMin.y) o.y /= pMax.y - pMin.y;
			if (pMax.z > pMin.z) o.z /= pMax.z - pMin.z;
			return o;
		}

		float surface_area() {
			TWT::Vector3f d = pMax - pMin;
			return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
		}

		int maximum_extent() {
			TWT::Vector3f d = pMax - pMin;
			if (d.x > d.y && d.x > d.z)
				return 0;
			else if (d.y > d.z)
				return 1;
			else
				return 2;
		}

		static constexpr float MachineEpsilon = std::numeric_limits<float>::epsilon() * 0.5;

		inline static constexpr float gamma(int n) {
			return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
		}

		inline TWT::Vector3f& operator[](int i) {
			return (i == 0) ? pMin : pMax;
		}

		inline const TWT::Vector3f& operator[](int i) const {
			return (i == 0) ? pMin : pMax;
		}

		//bool intersect(const Ray& ray, const TWT::Vector3f& invDir, const int dirIsNeg[3]) {
		//	const Bounds& bounds = *this;
		//// Check for ray intersection against $x$ and $y$ slabs
		//	float tMin = (bounds[dirIsNeg[0]].x - ray.origin.x) * invDir.x;
		//	float tMax = (bounds[1 - dirIsNeg[0]].x - ray.origin.x) * invDir.x;
		//	float tyMin = (bounds[dirIsNeg[1]].y - ray.origin.y) * invDir.y;
		//	float tyMax = (bounds[1 - dirIsNeg[1]].y - ray.origin.y) * invDir.y;

		//	// Update _tMax_ and _tyMax_ to ensure robust bounds intersection
		//	tMax *= 1 + 2 * gamma(3);
		//	tyMax *= 1 + 2 * gamma(3);
		//	if (tMin > tyMax || tyMin > tMax) return false;
		//	if (tyMin > tMin) tMin = tyMin;
		//	if (tyMax < tMax) tMax = tyMax;

		//	// Check for ray intersection against $z$ slab
		//	float tzMin = (bounds[dirIsNeg[2]].z - ray.origin.z) * invDir.z;
		//	float tzMax = (bounds[1 - dirIsNeg[2]].z - ray.origin.z) * invDir.z;

		//	// Update _tzMax_ to ensure robust bounds intersection
		//	tzMax *= 1 + 2 * gamma(3);
		//	if (tMin > tzMax || tzMin > tMax) return false;
		//	if (tzMin > tMin) tMin = tzMin;
		//	if (tzMax < tMax) tMax = tzMax;
		//	return (tMin < 1000) && (tMax > 0);
		//}

		//bool intersect(const Ray& ray, float& distan) {
		//	float t0 = 0, t1 = 1000.0;
		//	for (int i = 0; i < 3; ++i) {
		//		// Update interval for _i_th bounding box slab
		//		float invRayDir = 1 / ray.dir[i];
		//		float tNear = (pMin[i] - ray.origin[i]) * invRayDir;
		//		float tFar = (pMax[i] - ray.origin[i]) * invRayDir;

		//		// Update parametric interval from slab intersection $t$ values
		//		if (tNear > tFar) std::swap(tNear, tFar);

		//		// Update _tFar_ to ensure robust ray--bounds intersection
		//		tFar *= 1 + 2 * gamma(3);
		//		t0 = tNear > t0 ? tNear : t0;
		//		t1 = tFar < t1 ? tFar : t1;
		//		if (t0 > t1) return false;
		//	}

		//	float dist = 0;
		//	if (t0 < dist) dist = t0;
		//	if (t1 < dist) dist = t1;
		//	distan = dist;
		//	//if (hitt1) *hitt1 = t1;
		//	return true;
		//}

		static Bounds Union(Bounds b, TWT::Vector3f p) {
			return Bounds(glm::min(b.pMin, p), glm::max(b.pMax, p));
		}

		static Bounds Union(Bounds b, Bounds b2) {
			return Bounds(glm::min(b.pMin, b2.pMin), glm::max(b.pMax, b2.pMax));
		}
	};

	struct LBVHNode {
		Bounds bounds = Bounds(TWT::Vector3f(FLT_MAX), TWT::Vector3f(-FLT_MAX));
		TWT::UInt primitive_index = 0;
		TWT::UInt parent = 0;
		TWT::UInt left_child = 0;
		TWT::UInt right_child = 0;
	};

	inline Matrix4f Translate(Matrix4f Matrix, TWT::Vector3f Position) {
		return glm::translate(Matrix, Position);
	}

	inline Matrix4f Rotate(Matrix4f Matrix, TWT::Float Angle, TWT::Vector3f Axis) {
		return glm::rotate(Matrix, Angle, Axis);
	}
}