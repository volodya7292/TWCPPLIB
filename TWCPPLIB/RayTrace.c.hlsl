#include "HLSLHelper.hlsli"

struct InputData {
	uint gvb_vertex_offset;
	uint primitive_count;
};

struct Camera {
	float4 pos;
	float4x4 proj;
	float4x4 view;
	float4x4 proj_view;
};

StructuredBuffer<Vertex> gvb : register(t0);
StructuredBuffer<LBVHNode> nodes : register(t1);
RWTexture2D<float4> rt_output : register(u0);
ConstantBuffer<InputData> input : register(b0);
ConstantBuffer<Camera> camera : register(b1);

bool check_intersection_tri(in Ray ray, float3 v0, float3 v1, float3 v2) {
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

				//float3 interPoint = ray.origin + ray.dir * t;
				//float3 n = dot(normal, normalize(ray.origin - interPoint)) >= 0 ? normal : normal * float3(-1);

				if (t > EPSILON)
					return true;
					//return { true, length(ray.dir * t), interPoint, n, material, this };

				//else // This means that there is a line intersection but not a ray intersection.
				//	return { false, 1000, float3(0) };
			}
		}
	}

	//return { false, 1000, float3(0) };
	return false;
}

bool check_intersection(in Ray ray, uint prim_index) {
	float3 v0 = gvb[prim_index * 3].pos;
	float3 v1 = gvb[prim_index * 3 + 1].pos;
	float3 v2 = gvb[prim_index * 3 + 2].pos;
	return check_intersection_tri(ray, v0, v1, v2);
}

#define StackSize 64
float4 findNearestIntersection(Ray ray) {
	float distance;

	uint stackNodes[StackSize];

	unsigned int stackIndex = 0;

	stackNodes[stackIndex++] = uint(-1);

	uint childL, childR, node = 0;

	float d2 = 0;
	bool found = nodes[node].bounds.intersect(ray, d2);

	if (!found) {
		//return minInter;
		return float4(0, 0, 0, 0);
	} /*else if (node->shape) {
		Intersection inter = node->shape->check_intersection(ray);
		if (inter.intersected)
			minInter = inter;
	}*/

	bool lIntersection, rIntersection, traverseL, traverseR;
	
	uint co = 0;
	while (node != uint(-1)) {
		lIntersection = rIntersection = traverseL = traverseR = false;

		childL = nodes[node].left_child;
		if (childL != uint(-1)) {
			lIntersection = nodes[childL].bounds.intersect(ray, distance);

			if (lIntersection) {
				//return float4(1, 1, 1, 1);
				// Leaf node
				if (nodes[childL].primitive_index != uint(-1)) {
					if (check_intersection(ray, nodes[childL].primitive_index))
						return float4(1, 1, 1, 1);
					//Intersection inter = childL->shape->check_intersection(ray);
					////intersectionFound = childL->shape->intersection(ray, &curr);

					//if (inter.intersected && (inter.distance < minInter.distance)) {
					//	minInter = inter;
					//}

				} else {
					traverseL = true;
				}
			}
		}

		childR = nodes[node].right_child;
		if (childR != uint(-1)) {
			rIntersection = nodes[childR].bounds.intersect(ray, distance);

			if (rIntersection) {
				//return float4(1, 1, 1, 1);
				// Leaf node
				if (nodes[childR].primitive_index != uint(-1)) {
					if (check_intersection(ray, nodes[childR].primitive_index))
						return float4(1, 1, 1, 1);
					/*Intersection inter = childR->shape->check_intersection(ray);

					if (inter.intersected && (inter.distance < minInter.distance)) {
						minInter = inter;
					}*/

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

	return float4(0, 0, 0, 0);
}


float4 findNearestIntersection2(in Ray ray) {
	for (uint i = 0; i < input.primitive_count; i++) {
		bool inter = check_intersection(ray, i);
		if (inter)
			return float4(1, 1, 1, 1);

	}

	return float4(0, 0, 0, 0);
}

[numthreads(THREAD_GROUP_WIDTH, THREAD_GROUP_HEIGHT, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	const int WIDTH = 1280;
	const int HEIGHT = 720;
	const int WIDTH2 = WIDTH / 2;
	const int HEIGHT2 = HEIGHT / 2;
	const float FOV = 45;
	const double WH_RATIO = (double)WIDTH / HEIGHT;
	const float W_RATIO = tan(radians(FOV) / 2.0) * WH_RATIO;
	const float H_RATIO = tan(radians(FOV) / 2.0);

	float2 xy = DTid.xy + 0.5f;
	float2 screenPos = xy / float2(WIDTH, HEIGHT) * 2.0f - 1.0f;
	screenPos.y = -screenPos.y;
	screenPos *= float2(W_RATIO, H_RATIO);
	float3 dir = mul(float4(normalize(float3(screenPos, -1)), 1.0), camera.view).xyz;

	//int index = y * WIDTH + x;
	//randSeed = initRand(index, frameCount++, 16);
	/*float Px = ((x + 0.5f) / WIDTH2 - 1) * W_RATIO;
	float Py = (1 - (y + 0.5f) / HEIGHT2) * H_RATIO;
	vec3 dir = vec3(vec4(normalize(vec3(Px, Py, -1)), 1.0) * CAM_MATRIX);*/

	Ray r;
	r.origin = camera.pos;
	r.dir = dir;

	//trace_screen_ray({ CAM_POS, dir }, index);
	float4 color = findNearestIntersection(r);
	//float4 color = float4(1, 0, 1, 1);

	//tet g = { float4(DTid.x / 1280.0, 0, 0, 1), float4(0, DTid.x / 1280.0, 0, 1) };
	//buffer[DTid.x] = g;
	//OutputTexture[DTid.xy] = float4(normalize(DTid.xy + DTid.yx), normalize(DTid.yx).x, 1);
	rt_output[DTid.xy] = color;
}