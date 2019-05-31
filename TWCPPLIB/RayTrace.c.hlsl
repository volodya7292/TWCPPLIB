#include "RayTracing.hlsli"

struct InputData {
	uint use_two_scenes;
	uint light_source_count;
};

// Default scale scene
GVB       gvb : register(t0);
RTScene scene : register(t1);
RTNB      gnb : register(t2);
LSB       lsb : register(t3);

// Large scale scene
GVB       l_gvb : register(t4);
RTScene l_scene : register(t5);
RTNB      l_gnb : register(t6);
LSB       l_lsb : register(t7);


// Texture resources
Texture2DArray<float4> diffuse_tex : register(t8);
Texture2DArray<float4> specular_tex : register(t9);
Texture2DArray<float4> emission_tex : register(t10);
Texture2DArray<float4> normal_tex : register(t11);

// GBuffer resources
Texture2D<float4> g_position : register(t12);
Texture2D<float4> g_normal : register(t13);
Texture2D<float4> g_diffuse : register(t14);
Texture2D<float4> g_specular : register(t15);
Texture2D<float4> g_emission : register(t16);
sampler sam : register(s0);

// Output image
RWTexture2D<float4> rt_output : register(u0);

// Camera
ConstantBuffer<Camera> camera : register(b0);
// Scene data
ConstantBuffer<InputData> input : register(b1);

inline float3 to_large_scene_scale(float3 p) {
	float3 to_p = p - camera.pos.xyz;
	return camera.pos.xyz + normalize(to_p) * (length(to_p) * camera.info.y); // camera.info.y - scale factor for large objects
}

inline float3 to_default_scene_scale(float3 p) {
	float3 to_p = p - camera.pos.xyz;
	return camera.pos.xyz + normalize(to_p) * (length(to_p) / camera.info.y); // camera.info.y - scale factor for large objects
}

inline void TraceRayNormal(in Ray Ray, out TriangleIntersection TriInter) {
	TraceRay(scene, gnb, gvb, Ray, TriInter);
}

inline void TraceRayLarge(in Ray Ray, out TriangleIntersection TriInter) {
	TraceRay(l_scene, l_gnb, l_gvb, Ray, TriInter);
}

inline Ray primary_ray(uint2 screen_size, uint2 screen_pixel_pos) {
	const float ct = tan(camera.info.x / 2.0f);
	const float2 screenPos = ((screen_pixel_pos + 0.5f) / (float2)screen_size * 2.0f - 1.0f) * float2(ct * ((float)screen_size.x / screen_size.y), -ct);

	Ray ray;
	ray.origin = camera.pos.xyz;
	ray.dir = mul(float4(normalize(float3(screenPos, -1)), 1.0), camera.view).xyz;

	return ray;
}

[numthreads(THREAD_GROUP_WIDTH, THREAD_GROUP_HEIGHT, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint2 SIZE;
	rt_output.GetDimensions(SIZE.x, SIZE.y);

	float4 pos = g_position.SampleLevel(sam, DTid.xy / (float2)SIZE, 0);
	float4 normal = g_normal.SampleLevel(sam, DTid.xy / (float2)SIZE, 0);
	float4 diffuse = g_diffuse.SampleLevel(sam, DTid.xy / (float2)SIZE, 0);
	float4 specular = g_specular.SampleLevel(sam, DTid.xy / (float2)SIZE, 0);
	float4 emission = g_emission.SampleLevel(sam, DTid.xy / (float2)SIZE, 0);

	float4 color = float4(0, 0, 0, 0);

	if (pos.w > 0.9f) { // Not a background pixel
		Ray pRay = primary_ray(SIZE, DTid.xy);

		TriangleIntersection tri_inter;
		if (input.use_two_scenes == 1u)
			TraceRayLarge(pRay, tri_inter);
		else
			TraceRayNormal(pRay, tri_inter);

		//float4 color = float4(1, 0.3, 0.5, 1);
			
		if (tri_inter.Intersected) {
			color = diffuse_tex.SampleLevel(sam, float3(tri_inter.Bary, 0), 0);
		}

	} else { // Background pixel
		float4 e1 = diffuse_tex.SampleLevel(sam, float3(DTid.xy / (float2)SIZE, 0), 0);
		float4 e = emission_tex.SampleLevel(sam, float3(DTid.xy / (float2)SIZE, 0), 0);
		float4 e2 = normal_tex.SampleLevel(sam, float3(DTid.xy / (float2)SIZE, 0), 0);
		float4 e3 = specular_tex.SampleLevel(sam, float3(DTid.xy / (float2)SIZE * e.x * e2.x, 0), 0);
		color = float4(normal.x, diffuse.y, specular.z, emission.a) * lsb[0].info * l_lsb[0].info * e * e2 * e3 * e1;
	}

	//if (pos.a == 0.5f) {
	//	color = float4(normal.x, diffuse.y, specular.z, emission.a) * lsb[0].info * l_lsb[0].info;
	//} else {

	//	Ray pRay = primary_ray(SIZE, DTid.xy);

	//	TriangleIntersection tri_inter;
	//	bool intersected;
	//	if (input.use_two_scenes == 1u)
	//		intersected = TraceRayLarge(pRay, tri_inter);
	//	else
	//		intersected = TraceRayNormal(pRay, tri_inter);

	//	//float4 color = float4(1, 0.3, 0.5, 1);
	//	if (input.use_two_scenes == 1u) {
	//		float4 e = emission_tex.SampleLevel(sam, float3(tri_inter.Bary, 0), 0);
	//		float4 e2 = normal_tex.SampleLevel(sam, float3(tri_inter.Bary, 0), 0);
	//		color = specular_tex.SampleLevel(sam, float3(tri_inter.Bary * e.x * e2.x, 0), 0);
	//	}
	//	if (intersected) {
	//		color.xyz = diffuse_tex.SampleLevel(sam, float3(tri_inter.Bary, 0), 0);
	//	}
	//}

	rt_output[DTid.xy] = color;
}