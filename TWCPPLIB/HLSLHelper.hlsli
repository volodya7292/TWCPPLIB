#define THREAD_GROUP_WIDTH 8
#define THREAD_GROUP_HEIGHT 8
#define THREAD_GROUP_1D_WIDTH (THREAD_GROUP_WIDTH * THREAD_GROUP_HEIGHT)

#define MachineEpsilon 5.96e-08

struct Vertex {
	float3       pos : POSITION;
	float2 tex_coord : TEXCOORD;
	float3    normal : NORMAL;
};

struct Ray {
	float3 origin;
	float3 dir;
};

struct Bounds {
	float3 pMin;
	float3 pMax;

	inline static float gamma(int n) {
		return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
	}

	inline float3 i(int i) {
		return (i == 0) ? pMin : pMax;
	}

	bool intersect(in Ray ray, out float distan) {
		float t0 = 0, t1 = 1000.0;
		for (int i = 0; i < 3; ++i) {
			// Update interval for _i_th bounding box slab
			float invRayDir = 1 / ray.dir[i];
			float tNear = (pMin[i] - ray.origin[i]) * invRayDir;
			float tFar = (pMax[i] - ray.origin[i]) * invRayDir;

			// Update parametric interval from slab intersection $t$ values
			if (tNear > tFar) {
				float temp = tFar;
				tFar = tNear;
				tNear = temp;
			}

			// Update _tFar_ to ensure robust ray--bounds intersection
			tFar *= 1 + 2 * gamma(3);
			t0 = tNear > t0 ? tNear : t0;
			t1 = tFar < t1 ? tFar : t1;
			if (t0 > t1) return false;
		}

		float dist = 0;
		if (t0 < dist) dist = t0;
		if (t1 < dist) dist = t1;
		distan = dist;
		//if (hitt1) *hitt1 = t1;
		return true;
	}
};

struct LBVHNode {
	Bounds bounds;
	uint primitive_index;
	uint parent;
	uint left_child;
	uint right_child;
};