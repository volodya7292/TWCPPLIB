#include "HLSLHelper.hlsli"

static uint g_stack_nodes[32];
static uint g_stack_nodes2[32];

#define INTERSECTION_FLAG_NORMAL      1 << 0
#define INTERSECTION_FLAG_TEXCOORD    1 << 1

struct TriangleIntersection {
	bool   Intersected;
	uint   TriangleId;
	float3 Point;
	float3 Normal;
	float2 TexCoord;
	float  Distance;
	uint   Flags;

	inline void init(uint Flags) {
		Intersected = false;
		TriangleId = -1;
		Point = float3(0, 0, 0);
		Normal = float3(0, 0, 0);
		TexCoord = float2(0, 0);
		Distance = FLT_MAX;
		this.Flags = Flags;
	}
};

void triangle_intersection(in Ray ray, in GVB gvb, inout TriangleIntersection tri_inter) {
	const float3 v0 = gvb[tri_inter.TriangleId * 3].pos;
	const float3 v1 = gvb[tri_inter.TriangleId * 3 + 1].pos;
	const float3 v2 = gvb[tri_inter.TriangleId * 3 + 2].pos;

	const float3 edge1 = v1 - v0;
	const float3 edge2 = v2 - v0;
	const float3 h = cross(ray.dir, edge2);
	const float a = dot(edge1, h);

	if (abs(a) >= MachineEpsilon) {
		const float f = 1 / a;
		const float3 s = ray.origin - v0;
		const float u = f * dot(s, h);

		if (u >= 0.0 && u < 1.0) {
			const float3 q = cross(s, edge1);
			const float v = f * dot(ray.dir, q);

			if (v >= 0.0 && u + v <= 1.0f) {
				const float t = f * dot(edge2, q);

				if (t > MachineEpsilon) {
					tri_inter.Intersected = true;
					tri_inter.Point = ray.origin + ray.dir * t;
					tri_inter.Distance = length(ray.dir * t);

					if (tri_inter.Flags & INTERSECTION_FLAG_TEXCOORD) {
						tri_inter.TexCoord =
							gvb[tri_inter.TriangleId * 3].tex_coord.xy * (1 - u - v) +
							gvb[tri_inter.TriangleId * 3 + 1].tex_coord.xy * u +
							gvb[tri_inter.TriangleId * 3 + 2].tex_coord.xy * v;
					}
					if (tri_inter.Flags & INTERSECTION_FLAG_NORMAL) {
						const float3 normal = gvb[tri_inter.TriangleId * 3].normal;
						tri_inter.Normal = dot(normal, normalize(ray.origin - tri_inter.Point)) >= 0 ? normal : -normal;
					}
				}
			}
		}
	}


	//tri_inter.Point = float3(0, 0, 0);
	//tri_inter.Distance = 65535;


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

	//tri_inter.Point = interPoint;
	//tri_inter.Distance = distance(ray.origin, interPoint);
}


void mesh_rtas_trace_ray(in RTNB rtas, in uint vertex_offset, in uint node_offset, in GVB gvb, in Ray ray, inout TriangleIntersection tri_inter) {
	TriangleIntersection mininter = tri_inter, tempinter = tri_inter;

	float distance = 0;


	uint stackIndex = 0;
	g_stack_nodes[stackIndex++] = -1;

	uint childL, childR, elementIndex, node = node_offset;
	uint vo = vertex_offset / 3;


	float d2 = 0;
	bool found = rtas[node].bounds.intersect(ray, d2);

	elementIndex = rtas[node].element_index;

	if (!found) {
		tri_inter = mininter;
		return;
	} else if (elementIndex != -1) {
		tempinter.TriangleId = vo + elementIndex;
		triangle_intersection(ray, gvb, tempinter);
		if (tempinter.Intersected)
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
					tempinter.TriangleId = vo + elementIndex;
					triangle_intersection(ray, gvb, tempinter);
					if (tempinter.Intersected && tempinter.Distance < mininter.Distance)
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
					tempinter.TriangleId = vo + elementIndex;
					triangle_intersection(ray, gvb, tempinter);
					if (tempinter.Intersected && tempinter.Distance < mininter.Distance)
						mininter = tempinter;
				} else {
					traverseR = true;
				}
			}
		}

		if (!traverseL && !traverseR) {
			node = g_stack_nodes[--stackIndex]; // pop
		} else {
			node = (traverseL) ? childL : childR;
			if (traverseL && traverseR)
				g_stack_nodes[stackIndex++] = childR; // push
		}
	}

	tri_inter = mininter;
}

inline void mesh_transform_ray(in Ray ray, in float4x4 inverse_transform, out Ray new_ray) {
	new_ray.origin = mul(inverse_transform, float4(ray.origin, 1)).xyz;
	new_ray.dir = normalize(mul(inverse_transform, float4(ray.dir, 0)).xyz);
}

void check_scene_rtas_intersection(in RTNB gnb, in GVB gvb, in Ray ray, in uint rtas_vertex_index, in uint rtas_node_index, in float4x4 inverse_transform, inout TriangleIntersection tri_inter) {
	Ray nr;
	mesh_transform_ray(ray, inverse_transform, nr);
	mesh_rtas_trace_ray(gnb, rtas_vertex_index, rtas_node_index, gvb, nr, tri_inter);
}

void TraceRay(in RTScene SceneAS, in RTNB GNB, in GVB GVB, in Ray Ray, inout TriangleIntersection TriInter) {
	float bounds_distance = FLT_MAX;
	TriInter.Distance = FLT_MAX;
	TriangleIntersection mininter = TriInter, tempinter = TriInter;

	uint stackIndex = 0;
	g_stack_nodes2[stackIndex++] = -1;

	uint childL, childR, nodeOffset, node = 0;

	float d2 = 0;
	bool found = SceneAS[node].bounds.intersect(Ray, d2);

	if (!found) {
		TriInter = mininter;
		return;
	}

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
					if (tempinter.Distance < mininter.Distance)
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
					if (tempinter.Distance < mininter.Distance)
						mininter = tempinter;
				} else {
					traverseR = true;
				}
			}
		}

		if (!traverseL && !traverseR) {
			node = g_stack_nodes2[--stackIndex]; // pop
		} else {
			node = (traverseL) ? childL : childR;
			if (traverseL && traverseR)
				g_stack_nodes2[stackIndex++] = childR; // push
		}
	}

	TriInter = mininter;
}