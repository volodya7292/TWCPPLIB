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
	float3       pos : POSITION;
	float2 tex_coord : TEXCOORD;
	float3    normal : NORMAL;
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

	inline bool intersect(in Ray ray, out float distan) {
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
};

typedef StructuredBuffer<SceneLBVHNode> RTScene;
typedef StructuredBuffer<LBVHNode> RTNB;   // Ray Tracing Node Buffer
typedef StructuredBuffer<Vertex>   GVB;    // Global Vertex Buffer
typedef StructuredBuffer<float4x4> GMB;    // Global Matrix Buffer


// -----------------------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------------------------

struct Triangle {
	float3 v0, v1, v2;
};

struct TriangleIntersection {
	uint   TriangleID;
	float3 intersectionPoint;
	float  intersectionDistance;
};

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

bool triangle_intersection(in Ray ray, in Triangle tri, inout TriangleIntersection tri_inter) {
	const float EPSILON = 1e-8f;
	const float3 edge1 = tri.v1 - tri.v0;
	const float3 edge2 = tri.v2 - tri.v0;
	const float3 h = cross(ray.dir, edge2);
	const float a = dot(edge1, h);

	if (abs(a) >= EPSILON) {
		const float f = 1 / a;
		const float3 s = ray.origin - tri.v0;
		const float u = f * dot(s, h);

		if (u >= 0.0 && u < 1.0) {
			const float3 q = cross(s, edge1);
			const float v = f * dot(ray.dir, q);

			if (v >= 0.0 && u + v <= 1.0f) {
				const float t = f * dot(edge2, q);

				//float3 n = dot(normal, normalize(ray.origin - interPoint)) >= 0 ? normal : normal * float3(-1);

				if (t > EPSILON) {
					tri_inter.intersectionPoint = ray.origin + ray.dir * t;
					tri_inter.intersectionDistance = length(ray.dir * t);
					return true;
				}
			}
		}
	}

	tri_inter.intersectionPoint = float3(0, 0, 0);
	tri_inter.intersectionDistance = 65535;
	return false;


	//tri_inter.intersectionPoint = float3(0, 0, 0);
	//tri_inter.intersectionDistance = 65535;


	//uint zIndex = max_dim(ray.dir);
	//uint3 k;
	//k.x = (zIndex + 1) % 3;
	//k.y = (zIndex + 2) % 3;
	//k.z = zIndex;
	//if (ray.dir[k.z] < 0.0f) {
	//	int sw = k.y;
	//	k.y = k.x;
	//	k.x = sw;
	//}

	//float3 shear;
	//shear.x = ray.dir[k.x] / ray.dir[k.z];
	//shear.y = ray.dir[k.y] / ray.dir[k.z];
	//shear.z = 1.0f / ray.dir[k.z];

	//float3 A = Swizzle(v0 - ray.origin, k);
	//float3 B = Swizzle(v1 - ray.origin, k);
	//float3 C = Swizzle(v2 - ray.origin, k);

	//A.x = A.x - shear.x * A.z;
	//A.y = A.y - shear.y * A.z;		
	//B.x = B.x - shear.x * B.z;
	//B.y = B.y - shear.y * B.z;
	//C.x = C.x - shear.x * C.z;
	//C.y = C.y - shear.y * C.z;

	//float U = C.x * B.y - C.y * B.x;
	//float V = A.x * C.y - A.y * C.x;
	//float W = B.x * A.y - B.y * A.x;

	//float det = U + V + W;
	//if ((U < 0.0f || V < 0.0f || W < 0.0f) && (U > 0.0f || V > 0.0f || W > 0.0f)) {
	//	return false;
	//}

	//if (det == 0.0f) return false;
	//A.z = shear.z * A.z;
	//B.z = shear.z * B.z;
	//C.z = shear.z * C.z;
	//const float T = U * A.z + V * B.z + W * C.z;

	//float signCorrectedT = abs(T);
	//if (IsPositive(T) != IsPositive(det))
	//	signCorrectedT = -signCorrectedT;
	//if (signCorrectedT < 0.0f)
	//	return false;

	//float rcpDet = 1.0f / det;
	//float hitT = T * rcpDet;
	//float3 interPoint = ray.origin + ray.dir * hitT;
	////vec3 n = dot(normal, normalize(ray.origin - interPoint)) >= 0 ? normal : normal * vec3(-1);

	//tri_inter.intersectionPoint = interPoint;
	//tri_inter.intersectionDistance = distance(ray.origin, interPoint);

	return true;
}

inline bool triangle_intersection(in Ray ray, in GVB gvb, in uint prim_index, out TriangleIntersection tri_inter) {
	Triangle tri;
	tri.v0 = gvb[prim_index * 3].pos;
	tri.v1 = gvb[prim_index * 3 + 1].pos;
	tri.v2 = gvb[prim_index * 3 + 2].pos;
	tri_inter.TriangleID = prim_index;
	return triangle_intersection(ray, tri, tri_inter);
}

bool mesh_rtas_trace_ray(in RTNB rtas, in uint vertex_offset, in uint node_offset, in GVB gvb, in Ray ray, out TriangleIntersection tri_inter) {
	TriangleIntersection mininter, curr, tempinter;
	mininter.TriangleID = -1;
	mininter.intersectionPoint = float3(0, 0, 0);
	mininter.intersectionDistance = FLT_MAX;
	tempinter.TriangleID = -1;
	tempinter.intersectionPoint = float3(0, 0, 0);
	tempinter.intersectionDistance = FLT_MAX;
	float distance = 0;

	uint stackNodes[64];
	unsigned int stackIndex = 0;
	stackNodes[stackIndex++] = -1;

	uint childL, childR, elementIndex, node = node_offset;
	uint vo = vertex_offset / 3;


	float d2 = 0;
	bool found = rtas[node].bounds.intersect(ray, d2);

	tri_inter.TriangleID = -1;
	tri_inter.intersectionPoint = float3(0, 0, 0);
	tri_inter.intersectionDistance = FLT_MAX;

	elementIndex = rtas[node].element_index;

	if (!found) {
		return false;
	} else if (elementIndex != -1) {
		if (triangle_intersection(ray, gvb, vo + elementIndex, tempinter))
			mininter = tempinter;
	}

	bool lintersection, rintersection, traverseL, traverseR;

	[loop]
	while (node != -1) {
		lintersection = rintersection = traverseL = traverseR = false;

		childL = node_offset + rtas[node].left_child;
		if (childL != -1) {
			lintersection = rtas[childL].bounds.intersect(ray, distance);

			if (lintersection) {
				elementIndex = rtas[childL].element_index;

				if (elementIndex != -1) {
					if (triangle_intersection(ray, gvb, vo + elementIndex, tempinter))
						if (tempinter.intersectionDistance < mininter.intersectionDistance)
							mininter = tempinter;
				} else {
					traverseL = true;
				}
			}
		}

		childR = node_offset + rtas[node].right_child;
		if (childR != -1) {
			rintersection = rtas[childR].bounds.intersect(ray, distance);

			if (rintersection) {
				elementIndex = rtas[childR].element_index;

				if (elementIndex != -1) {
					if (triangle_intersection(ray, gvb, vo + elementIndex, tempinter))
						if (tempinter.intersectionDistance < mininter.intersectionDistance)
							mininter = tempinter;
				} else {
					traverseR = true;
				}
			}
		}

		if (!traverseL && !traverseR) {
			node = stackNodes[--stackIndex]; // pop

		} else {
			node = (traverseL) ? childL : childR;
			if (traverseL && traverseR)
				stackNodes[stackIndex++] = childR; // push
		}
	}

	tri_inter = mininter;
	return mininter.intersectionDistance != FLT_MAX;
}

inline void mesh_transform_ray(in Ray ray, in float4x4 inverse_transform, out Ray new_ray) {
	new_ray.origin = mul(inverse_transform, float4(ray.origin, 1)).xyz;
	new_ray.dir = normalize(mul(inverse_transform, float4(ray.dir, 0)).xyz);
}

inline void check_scene_rtas_intersection(in RTNB gnb, in GVB gvb, in Ray ray, in uint rtas_vertex_index, in uint rtas_node_index, in float4x4 inverse_transform, inout TriangleIntersection tri_inter) {
	Ray nr;
	mesh_transform_ray(ray, inverse_transform, nr);
	mesh_rtas_trace_ray(gnb, rtas_vertex_index, rtas_node_index, gvb, nr, tri_inter);
}

bool TraceRay(in RTScene SceneAS, in RTNB GNB, in GVB GVB, in Ray Ray, out TriangleIntersection Triinter) {
	float bounds_distance = FLT_MAX;
	TriangleIntersection mininter, curr, tempinter;
	mininter.TriangleID = -1;
	mininter.intersectionPoint = float3(0, 0, 0);
	mininter.intersectionDistance = FLT_MAX;
	tempinter.TriangleID = -1;
	tempinter.intersectionPoint = float3(0, 0, 0);
	tempinter.intersectionDistance = FLT_MAX;
	Triinter.TriangleID = -1;
	Triinter.intersectionPoint = float3(0, 0, 0);
	Triinter.intersectionDistance = FLT_MAX;

	uint stackNodes[64];
	unsigned int stackIndex = 0;
	stackNodes[stackIndex++] = -1;

	uint childL, childR, nodeOffset, node = 0;

	float d2 = 0;
	bool found = SceneAS[node].bounds.intersect(Ray, d2);

	if (!found)
		return false;

	bool lintersection, rintersection, traverseL, traverseR;

	[loop]
	while (node != -1) {
		lintersection = rintersection = traverseL = traverseR = false;

		childL = SceneAS[node].left_child;
		if (childL != -1) {
			lintersection = SceneAS[childL].bounds.intersect(Ray, bounds_distance);

			if (lintersection) {
				nodeOffset = SceneAS[childL].instance.node_offset;

				if (nodeOffset != -1) {
					check_scene_rtas_intersection(GNB, GVB, Ray, SceneAS[childL].instance.vertex_offset, nodeOffset, SceneAS[childL].instance.transform_inverse, tempinter);
					if (tempinter.intersectionDistance < mininter.intersectionDistance)
						mininter = tempinter;
				} else {
					traverseL = true;
				}
			}
		}

		childR = SceneAS[node].right_child;
		if (childR != -1) {
			rintersection = SceneAS[childR].bounds.intersect(Ray, bounds_distance);

			if (rintersection) {
				nodeOffset = SceneAS[childR].instance.node_offset;

				if (nodeOffset != -1) {
					check_scene_rtas_intersection(GNB, GVB, Ray, SceneAS[childR].instance.vertex_offset, nodeOffset, SceneAS[childR].instance.transform_inverse, tempinter);
					if (tempinter.intersectionDistance < mininter.intersectionDistance)
						mininter = tempinter;
				} else {
					traverseR = true;
				}
			}
		}

		if (!traverseL && !traverseR) {
			node = stackNodes[--stackIndex]; // pop
		} else {
			node = (traverseL) ? childL : childR;
			if (traverseL && traverseR)
				stackNodes[stackIndex++] = childR; // push
		}
	}

	Triinter = mininter;
	return mininter.intersectionDistance != FLT_MAX;
}