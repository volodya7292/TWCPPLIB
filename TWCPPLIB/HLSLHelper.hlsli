#define THREAD_GROUP_WIDTH 8
#define THREAD_GROUP_HEIGHT 8
#define THREAD_GROUP_1D_WIDTH (THREAD_GROUP_WIDTH * THREAD_GROUP_HEIGHT)

#define FLT_MAX        3.402823466e+38f
#define MachineEpsilon 5.96e-08

float4x4 scale(float4x4 m, in float v) {
	m[0][0] *= v; m[1][0] *= v; m[2][0] *= v;
	m[0][1] *= v; m[1][1] *= v; m[2][1] *= v;
	m[0][2] *= v; m[1][2] *= v; m[2][2] *= v;
	m[0][3] *= v; m[1][3] *= v; m[2][3] *= v;
	return m;
}

float4x4 translate(float4x4 m, in float3 v) {
	m[0][3] = v.x;
	m[1][3] = v.y;
	m[2][3] = v.z;
	return m;
}

float3 translation(in float4x4 m) {
	return float3(m[0][3], m[1][3], m[2][3]);
}

inline bool equals(in float3 v0, in float v) {
	return v0.x == v && v0.y == v && v0.z == v;
}

inline bool not_equals(in float3 v0, in float v) {
	return v0.x != v && v0.y != v && v0.z != v;
}

inline bool less(in float3 v0, in float v) {
	return v0.x < v && v0.y < v && v0.z < v;
}

inline bool less(in float3 v0, in float3 v1) {
	return v0.x < v1.x && v0.y < v1.y && v0.z < v1.z;
}

inline bool greater(in float3 v0, in float3 v1) {
	return v0.x > v1.x && v0.y > v1.y && v0.z > v1.z;
}

float depth_linearize(float d, float zNear, float zFar) {
	return zNear * zFar / (zFar + d * (zNear - zFar));
}

float depth_delinearize(float ld, float zNear, float zFar) {
	return (zNear * zFar / ld - zFar) / (zNear - zFar);
}

struct Vertex {
	float3          pos : POSITION;
	float3    tex_coord : TEXCOORD; // .z - material ID
	float3       normal : NORMAL;
	float3      tangent : TANGENT;
	float3    bitangent : BITANGENT;
};

struct Ray {
	float3 origin;
	float3 dir;
};

struct Bounds {
	float4 pMin;
	float4 pMax;

	inline static float gamma(int n) {
		return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
	}

	inline float4 i(int i) {
		return (i == 0) ? pMin : pMax;
	}

	bool intersect(in Ray ray, out float distan) {
		float t0 = 0, t1 = FLT_MAX;
		distan = FLT_MAX;
		for (int i = 0; i < 3; ++i) {
			// Update interval for _i_th bounding box slab
			const float invRayDir = 1 / ray.dir[i];
			float tNear = (pMin[i] - ray.origin[i]) * invRayDir;
			float tFar = (pMax[i] - ray.origin[i]) * invRayDir;

			// Update parametric interval from slab intersection $t$ values
			if (tNear > tFar) {
				const float temp = tFar;
				tFar = tNear;
				tNear = temp;
			}

			// Update _tFar_ to ensure robust ray--bounds intersection
			tFar *= 1 + 2 * gamma(3);
			t0 = tNear > t0 ? tNear : t0;
			t1 = tFar < t1 ? tFar : t1;
			if (t0 > t1) return false;
		}

		distan = 0;
		if (t0 < distan) distan = t0;
		if (t1 < distan) distan = t1;
		//if (hitt1) *hitt1 = t1;
		return true;
	}
};

struct LBVHNode {
	Bounds bounds;
	uint element_index;
	uint parent;
	uint left_child;
	uint right_child;
};

struct SceneLBVHInstance {
	uint vertex_offset;
	uint node_offset;
	float4x4 transform;
	float4x4 transform_inverse;
};

struct SceneLBVHNode {
	SceneLBVHInstance instance;
	Bounds bounds;
	uint parent;
	uint left_child;
	uint right_child;
};

struct Camera {
	float4 pos;
	float4x4 proj;
	float4x4 view;
	float4x4 proj_view;
	float4 info; // .x - FOVy in radians, .y - scale factor (large objects are scaled down)
};

typedef StructuredBuffer<SceneLBVHNode> RTScene;
typedef StructuredBuffer<LBVHNode> RTNB;   // Ray Tracing Node Buffer
typedef StructuredBuffer<Vertex>   GVB;    // Global Vertex Buffer


// -----------------------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------------------------

inline static float3 swizzle(float3 v, int3 swizzleOrder) {
	return float3(v[swizzleOrder.x], v[swizzleOrder.y], v[swizzleOrder.z]);
}

inline static uint max_dim(float3 v) {
	float3 v2 = abs(v);

	if (v2.x > v2.y) {
		// y isn't the maximum, so it's either x or z
		if (v2.x > v2.z) {
			return 0;
		} else {
			return 2;
		}
	} else if (v2.y > v2.z) {
		// x isn't the maximum, so it's either y or z
		return 1;
	} else {
		return 2;
	}
}

inline float3 Swizzle(float3 v, int3 swizzleOrder) {
	return float3(v[swizzleOrder.x], v[swizzleOrder.y], v[swizzleOrder.z]);
}

inline bool IsPositive(float f) { return f > 0.0f; }