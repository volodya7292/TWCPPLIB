#define THREAD_GROUP_WIDTH 8
#define THREAD_GROUP_HEIGHT 8
#define THREAD_GROUP_1D_WIDTH (THREAD_GROUP_WIDTH * THREAD_GROUP_HEIGHT)

#define        FLT_MAX    3.402823466e+38f
#define MachineEpsilon    5.96e-08
#define             PI    3.141592653
#define            PI2    6.283185307

#define sqr(x) (x) * (x)

static uint rand_seed;

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

inline float3 translation(in float4x4 m) {
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

inline float depth_linearize(float d, float zNear, float zFar) {
	return zNear * zFar / (zFar + d * (zNear - zFar));
}

inline float depth_delinearize(float ld, float zNear, float zFar) {
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

struct LightSource {
	float4 pos;
	float4 color;
	float4 info; // .x - type, .y - triangle id, z - sphere radius
};

struct Camera {
	float4 pos;
	float4x4 proj;
	float4x4 view;
	float4x4 proj_view;
	float4 info; // .x - FOVy in radians, .y - scale factor (large objects are scaled down)
};

struct RendererInfoCB {
	uint4 info; // .x - frame index
};

typedef StructuredBuffer<SceneLBVHNode> RTScene;
typedef StructuredBuffer<LBVHNode>      RTNB;   // Ray Tracing Node Buffer
typedef StructuredBuffer<Vertex>        GVB;    // Global Vertex Buffer
typedef StructuredBuffer<LightSource>   LSB;    // Light Source Buffer


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

void rand_init(uint val0, uint val1, uint backoff = 16) {
	uint v0 = val0, v1 = val1, s0 = 0;

	for (uint n = 0; n < backoff; n++) {
		s0 += 0x9e3779b9;
		v0 += ((v1 << 4) + 0xa341316c) ^ (v1 + s0) ^ ((v1 >> 5) + 0xc8013ea4);
		v1 += ((v0 << 4) + 0xad90777d) ^ (v0 + s0) ^ ((v0 >> 5) + 0x7e95761e);
	}

	rand_seed = v0;
}

// Random range: [0;1]
inline float rand_next() {
	rand_seed = 1664525u * rand_seed + 1013904223u;
	return float(rand_seed & 0x00FFFFFF) / float(0x01000000);
}

inline bool rand_next_bool() {
	return round(rand_next());
}

float3 get_perpendicular_vector(float3 u) {
	float3 a = abs(u);
	uint xm = ((a.x - a.y) < 0 && (a.x - a.z) < 0) ? 1 : 0;
	uint ym = (a.y - a.z) < 0 ? (1 ^ xm) : 0;
	uint zm = 1 ^ (xm | ym);
	return cross(u, float3(xm, ym, zm));
}

// Get a cosine-weighted random vector centered around a specified normal direction.
float3 rand_cos_hemisphere_dir(float3 normal) {
	// Get 2 random numbers to select our sample with
	float2 randVal = float2(rand_next(), rand_next());

	// Cosine weighted hemisphere sample from RNG
	float3 bitangent = get_perpendicular_vector(normal);
	float3 tangent = cross(bitangent, normal);
	float r = sqrt(randVal.x);
	float phi = PI2 * randVal.y;

	// Get our cosine-weighted hemisphere lobe sample direction
	return tangent * (r * cos(phi).x) + bitangent * (r * sin(phi)) + normal.xyz * sqrt(max(0.0, 1.0f - randVal.x));
}
