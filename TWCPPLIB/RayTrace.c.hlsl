#include "HLSLHelper.hlsli"

struct Camera {
	float4 pos;
	float4x4 proj;
	float4x4 view;
	float4x4 proj_view;
};

GVB gvb : register(t0);
RTScene scene : register(t1);
RTNB gnb : register(t2);
RWTexture2D<float4> rt_output : register(u0);
ConstantBuffer<Camera> camera : register(b0);


//float4 findNearestIntersection2(in Ray ray) {
//	for (uint i = 0; i < input.primitive_count; i++) {
//		bool inter = check_intersection(ray, i);
//		if (inter)
//			return float4(1, 1, 1, 1);
//
//	}
//
//	return float4(0, 0, 0, 0);
//}



[numthreads(THREAD_GROUP_WIDTH, THREAD_GROUP_HEIGHT, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	const int WIDTH = 1280;
	const int HEIGHT = 720;
	const int WIDTH2 = WIDTH / 2;
	const int HEIGHT2 = HEIGHT / 2;
	const float FOV = 45;
	const float WH_RATIO = (float)WIDTH / HEIGHT;
	const float W_RATIO = tan(radians(FOV) / 2.0f) * WH_RATIO;
	const float H_RATIO = tan(radians(FOV) / 2.0f);

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
	r.origin = camera.pos.xyz;
	r.dir = dir;

	//trace_screen_ray({ CAM_POS, dir }, index);

	TriangleIntersection tri_inter;
	bool intersected = TraceRay(scene, gnb, gvb, r, tri_inter);

	float4 color = float4(1, 0.3, 0.5, 1);
	if (intersected)
		color = float4(abs(gvb[tri_inter.TriangleID * 3].normal * 0.5f), 1);
	//float4 color = float4(1, 0, 1, 1);

	//tet g = { float4(DTid.x / 1280.0, 0, 0, 1), float4(0, DTid.x / 1280.0, 0, 1) };
	//buffer[DTid.x] = g;
	//OutputTexture[DTid.xy] = float4(normalize(DTid.xy + DTid.yx), normalize(DTid.yx).x, 1);
	rt_output[DTid.xy] = color;
}