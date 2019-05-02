#define THREAD_GROUP_WIDTH 8
#define THREAD_GROUP_HEIGHT 8
#define THREAD_GROUP_1D_WIDTH (THREAD_GROUP_WIDTH * THREAD_GROUP_HEIGHT)

#define FLT_MAX        3.402823466e+38f
#define MachineEpsilon 5.96e-08

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
		float t0 = 0, t1 = FLT_MAX;
		distan = FLT_MAX;
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
	uint element_index;
	uint parent;
	uint left_child;
	uint right_child;
};

struct SceneLBVHNode {
	float4x4 transform;
	float4x4 transform_inverse;
	uint vertex_offset;
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

typedef StructuredBuffer<SceneLBVHNode> RTScene;
typedef StructuredBuffer<LBVHNode> RTNB; // Ray Tracing Node Buffer
typedef StructuredBuffer<Vertex>   GVB;  // Global Vertex Buffer
typedef StructuredBuffer<float4x4> GMB;  // Global Matrix Buffer


// -----------------------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------------------------


struct TriangleIntersection {
	uint   TriangleID;
	float3 IntersectionPoint;
	float  IntersectionDistance;
};

bool triangle_intersection(in Ray ray, in float3 v0, in float3 v1, in float3 v2, inout TriangleIntersection tri_inter) {
	float EPSILON = 1e-8f;
	float3 edge1 = v1 - v0;
	float3 edge2 = v2 - v0;
	float3 h = cross(ray.dir, edge2);
	float a = dot(edge1, h);

	if (abs(a) >= EPSILON) {
		float f = 1 / a;
		float3 s = ray.origin - v0;
		float u = f * dot(s, h);

		if (u >= 0.0 && u < 1.0) {
			float3 q = cross(s, edge1);
			float v = f * dot(ray.dir, q);

			if (v >= 0.0 && u + v <= 1.0f) {
				float t = f * dot(edge2, q);

				//float3 n = dot(normal, normalize(ray.origin - interPoint)) >= 0 ? normal : normal * float3(-1);

				if (t > EPSILON) {
					tri_inter.IntersectionPoint = ray.origin + ray.dir * t;
					tri_inter.IntersectionDistance = length(ray.dir * t);
					return true;
				}
			}
		}
	}

	tri_inter.IntersectionPoint = float3(0, 0, 0);
	tri_inter.IntersectionDistance = 65535;
	return false;
}

bool triangle_intersection(in Ray ray, in GVB gvb, in uint prim_index, out TriangleIntersection tri_inter) {
	float3 v0 = gvb[prim_index * 3].pos;
	float3 v1 = gvb[prim_index * 3 + 1].pos;
	float3 v2 = gvb[prim_index * 3 + 2].pos;
	tri_inter.TriangleID = prim_index;
	return triangle_intersection(ray, v0, v1, v2, tri_inter);
}

bool mesh_rtas_trace_ray(in RTNB rtas, in uint vertex_offset, in uint node_offset, in GVB gvb, in Ray ray, out TriangleIntersection tri_inter) {
	TriangleIntersection minInter, curr, tempInter;
	minInter.TriangleID = -1;
	minInter.IntersectionPoint = float3(0, 0, 0);
	minInter.IntersectionDistance = FLT_MAX;
	tempInter.TriangleID = -1;
	tempInter.IntersectionPoint = float3(0, 0, 0);
	tempInter.IntersectionDistance = FLT_MAX;
	float distance = 0;

	uint stackNodes[64];
	unsigned int stackIndex = 0;
	stackNodes[stackIndex++] = -1;

	uint childL, childR, node = node_offset;
	uint vo = vertex_offset / 3;


	float d2 = 0;
	bool found = rtas[node].bounds.intersect(ray, d2);

	tri_inter.TriangleID = -1;
	tri_inter.IntersectionPoint = float3(0, 0, 0);
	tri_inter.IntersectionDistance = FLT_MAX;

	if (!found) {
		return false;
	} else if (rtas[node].element_index != -1) {
		if (triangle_intersection(ray, gvb, vo + rtas[node].element_index, tempInter))
			minInter = tempInter;
	}

	bool lIntersection, rIntersection, traverseL, traverseR;

	while (node != -1) {
		lIntersection = rIntersection = traverseL = traverseR = false;

		childL = node_offset + rtas[node].left_child;
		if (childL != -1) {
			lIntersection = rtas[childL].bounds.intersect(ray, distance);

			if (lIntersection) {
				// Leaf node
				if (rtas[childL].element_index != -1) {
					if (triangle_intersection(ray, gvb, vo + rtas[childL].element_index, tempInter)) {
						if (tempInter.IntersectionDistance < minInter.IntersectionDistance)
							minInter = tempInter;
					}
				} else {
					traverseL = true;
				}
			}
		}

		childR = node_offset + rtas[node].right_child;
		if (childR != -1) {
			rIntersection = rtas[childR].bounds.intersect(ray, distance);

			if (rIntersection) {
				// Leaf node
				if (rtas[childR].element_index != -1) {
					if (triangle_intersection(ray, gvb, vo + rtas[childR].element_index, tempInter)) {
						if (tempInter.IntersectionDistance < minInter.IntersectionDistance)
							minInter = tempInter;
					}
				} else {
					traverseR = true;
				}
			}
		}

		if (!traverseL && !traverseR) {
			node = stackNodes[--stackIndex]; // pop

		} else {
			node = (traverseL) ? childL : childR;
			if (traverseL && traverseR) {
				stackNodes[stackIndex++] = childR; // push
			}
		}
	}

	tri_inter = minInter;
	return minInter.IntersectionDistance != FLT_MAX;
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

bool TraceRay(in RTScene SceneAS, in RTNB GNB, in GVB GVB, in Ray Ray, out TriangleIntersection TriInter) {
	float bounds_distance = FLT_MAX;
	TriangleIntersection minInter, curr, tempInter;
	minInter.TriangleID = -1;
	minInter.IntersectionPoint = float3(0, 0, 0);
	minInter.IntersectionDistance = FLT_MAX;
	tempInter.TriangleID = -1;
	tempInter.IntersectionPoint = float3(0, 0, 0);
	tempInter.IntersectionDistance = FLT_MAX;
	TriInter.TriangleID = -1;
	TriInter.IntersectionPoint = float3(0, 0, 0);
	TriInter.IntersectionDistance = FLT_MAX;


	uint stackNodes[64];
	unsigned int stackIndex = 0;
	stackNodes[stackIndex++] = -1;

	uint childL, childR, node = 0;

	float d2 = 0;
	bool found = SceneAS[node].bounds.intersect(Ray, d2);

	if (!found)
		return false;

	bool lIntersection, rIntersection, traverseL, traverseR;

	while (node != -1) {
		lIntersection = rIntersection = traverseL = traverseR = false;

		childL = SceneAS[node].left_child;
		if (childL != -1) {
			lIntersection = SceneAS[childL].bounds.intersect(Ray, bounds_distance);

			if (lIntersection) {
				// Leaf node
				if (SceneAS[childL].element_index != -1) {
					check_scene_rtas_intersection(GNB, GVB, Ray, SceneAS[childL].vertex_offset, SceneAS[childL].element_index, SceneAS[childL].transform_inverse, tempInter);
					if (tempInter.IntersectionDistance < minInter.IntersectionDistance)
						minInter = tempInter;
				} else {
					traverseL = true;
				}
			}
		}

		childR = SceneAS[node].right_child;
		if (childR != -1) {
			rIntersection = SceneAS[childR].bounds.intersect(Ray, bounds_distance);

			if (rIntersection) {
				// Leaf node
				if (SceneAS[childR].element_index != -1) {
					check_scene_rtas_intersection(GNB, GVB, Ray, SceneAS[childR].vertex_offset, SceneAS[childR].element_index, SceneAS[childR].transform_inverse, tempInter);
					if (tempInter.IntersectionDistance < minInter.IntersectionDistance)
						minInter = tempInter;
				} else {
					traverseR = true;
				}
			}
		}

		if (!traverseL && !traverseR) {
			node = stackNodes[--stackIndex]; // pop

		} else {
			node = (traverseL) ? childL : childR;
			if (traverseL && traverseR) {
				stackNodes[stackIndex++] = childR; // push
			}
		}
	}

	TriInter = minInter;

	//return bounds_distance != FLT_MAX;
	return minInter.IntersectionDistance != FLT_MAX;
}