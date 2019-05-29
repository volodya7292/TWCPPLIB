#include "HLSLHelper.hlsli"

struct InputData {
	uint use_two_scenes;
};

// Default scale scene
GVB gvb : register(t0);
RTScene scene : register(t1);
RTNB gnb : register(t2);

// Large scale scene
GVB l_gvb : register(t3);
RTScene l_scene : register(t4);
RTNB l_gnb : register(t5);

Texture2DArray<float4> diffuse_tex : register(t6);
Texture2DArray<float4> specular_tex : register(t7);
sampler sam : register(s0);

// Output image
RWTexture2D<float4> rt_output : register(u0);

// Camera
ConstantBuffer<Camera> camera : register(b0);
ConstantBuffer<InputData> input : register(b1);

inline float to_large_scene_scale(float3 p) {
	float3 to_p = p - camera.pos.xyz;
	return camera.pos.xyz + normalize(to_p) * (length(to_p) * camera.info.x); // camera.info.x - scale factor for large objects
}

inline float to_default_scene_scale(float3 p) {
	float3 to_p = p - camera.pos.xyz;
	return camera.pos.xyz + normalize(to_p) * (length(to_p) / camera.info.x); // camera.info.x - scale factor for large objects
}

inline bool TraceRayNormal(in Ray Ray, out TriangleIntersection TriInter) {
	return TraceRay(scene, gnb, gvb, Ray, TriInter);
}

inline bool TraceRayLarge(in Ray Ray, out TriangleIntersection TriInter) {
	return TraceRay(l_scene, l_gnb, l_gvb, Ray, TriInter);
}

[numthreads(THREAD_GROUP_WIDTH, THREAD_GROUP_HEIGHT, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
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

	Ray r;
	r.origin = camera.pos.xyz;
	r.dir = dir;

	TriangleIntersection tri_inter;
	bool intersected;
	if (input.use_two_scenes == 1u)
		intersected = TraceRayLarge(r, tri_inter);
	else
		intersected = TraceRayNormal(r, tri_inter);

	float4 color = float4(1, 0.3, 0.5, 1);
	if (input.use_two_scenes == 1u) {
		color = specular_tex.SampleLevel(sam, float3(tri_inter.Bary, 0), 0);
	}
	if (intersected) {
		color.xyz = diffuse_tex.SampleLevel(sam, float3(tri_inter.Bary, 0), 0);
	}

	rt_output[DTid.xy] = color;
}