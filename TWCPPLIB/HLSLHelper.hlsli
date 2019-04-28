#define THREAD_GROUP_WIDTH 8
#define THREAD_GROUP_HEIGHT 8
#define THREAD_GROUP_1D_WIDTH (THREAD_GROUP_WIDTH * THREAD_GROUP_HEIGHT)

#define FLT_MAX        3.402823466e+38f
#define MachineEpsilon 5.96e-08

bool less(in float3 v0, in float3 v1) {
	return v0.x < v1.x && v0.y < v1.y && v0.z < v1.z;
}

bool greater(in float3 v0, in float3 v1) {
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
	uint element_index;
	uint parent;
	uint left_child;
	uint right_child;
};

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
					//tri_inter.TriangleID = -1;
					tri_inter.IntersectionPoint = ray.origin + ray.dir * t;
					tri_inter.IntersectionDistance = length(ray.dir * t);
					return true;
				}
			}
		}
	}

	return false;
}

bool triangle_intersection(in Ray ray, in GVB gvb, in uint prim_index, out TriangleIntersection tri_inter) {
	float3 v0 = gvb[prim_index * 3].pos;
	float3 v1 = gvb[prim_index * 3 + 1].pos;
	float3 v2 = gvb[prim_index * 3 + 2].pos;
	tri_inter.TriangleID = prim_index;
	return triangle_intersection(ray, v0, v1, v2, tri_inter);
}

bool mesh_rtas_trace_ray(in RTNB rtas, in uint node_offset, in GVB gvb, in Ray ray, out TriangleIntersection tri_inter) {
	TriangleIntersection minInter, curr, tempInter;
	minInter.IntersectionDistance = 1000;
	float distance;

	uint stackNodes[64];
	unsigned int stackIndex = 0;
	stackNodes[stackIndex++] = -1;

	uint childL, childR, node = node_offset;

	float d2 = 0;
	bool found = rtas[node].bounds.intersect(ray, d2);

	if (!found) {
		return false;
	} else if (rtas[node].element_index != -1) {
		if (triangle_intersection(ray, gvb, rtas[node].element_index, tempInter))
			minInter = tempInter;
	}

	bool lIntersection, rIntersection, traverseL, traverseR;

	while (node != -1) {
		lIntersection = rIntersection = traverseL = traverseR = false;

		childL = rtas[node].left_child;
		if (childL != -1) {
			lIntersection = rtas[childL].bounds.intersect(ray, distance);

			if (lIntersection) {
				// Leaf node
				if (rtas[childL].element_index != -1) {
					if (triangle_intersection(ray, gvb, rtas[childL].element_index, tempInter)) {
						if (tempInter.IntersectionDistance < minInter.IntersectionDistance)
							minInter = tempInter;
					}
				} else {
					traverseL = true;
				}
			}
		}

		childR = rtas[node].right_child;
		if (childR != -1) {
			rIntersection = rtas[childR].bounds.intersect(ray, distance);

			if (rIntersection) {
				// Leaf node
				if (rtas[childR].element_index != -1) {
					if (triangle_intersection(ray, gvb, rtas[childR].element_index, tempInter)) {
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
	return minInter.IntersectionDistance != 1000;
}

bool scene_rtas_trace_ray(in RTNB rtas, in Ray ray, out uint rtas_node_index) {
	float minDistance = 1000;
	float distance;

	uint stackNodes[64];
	unsigned int stackIndex = 0;
	stackNodes[stackIndex++] = -1;

	uint childL, childR, node = 0;

	float d2 = 0;
	bool found = rtas[node].bounds.intersect(ray, d2);

	if (!found)
		return false;

	bool lIntersection, rIntersection, traverseL, traverseR;

	while (node != -1) {
		lIntersection = rIntersection = traverseL = traverseR = false;

		childL = rtas[node].left_child;
		if (childL != -1) {
			lIntersection = rtas[childL].bounds.intersect(ray, distance);

			if (lIntersection) {
				// Leaf node
				if (rtas[childL].element_index != -1) {
					if (distance < minDistance) {
						minDistance = distance;
						rtas_node_index = rtas[childL].element_index;
					}
				} else {
					traverseL = true;
				}
			}
		}

		childR = rtas[node].right_child;
		if (childR != -1) {
			rIntersection = rtas[childR].bounds.intersect(ray, distance);

			if (rIntersection) {
				// Leaf node
				if (rtas[childR].element_index != -1) {
					if (distance < minDistance) {
						minDistance = distance;
						rtas_node_index = rtas[childR].element_index;
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

	return minDistance != 1000;
}

bool TraceRay(in RTNB SceneAS, in RTNB gnb, in GVB gvb, in Ray ray, out TriangleIntersection tri_inter) {
	uint rtas_node_index;
	if (scene_rtas_trace_ray(SceneAS, ray, rtas_node_index))
		if (mesh_rtas_trace_ray(gnb, rtas_node_index, gvb, ray, tri_inter))
			return true;
	return false;
}
