#include "RayTracing.hlsli"

#define COS_45DEG 0.7071067811

struct InputData {
	uint use_two_scenes;
	uint def_scene_light_count;
	uint large_scene_light_count;
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
ConstantBuffer<RendererInfoCB> renderer : register(b2);


void load_texture_data(in float3 tex_coord, out float4 diffuse, out float4 specular, out float4 emission, out float3 normal) {
	diffuse = diffuse_tex.SampleLevel(sam, tex_coord, 0);
	specular = specular_tex.SampleLevel(sam, tex_coord, 0);
	emission = emission_tex.SampleLevel(sam, tex_coord, 0);
	normal = normal_tex.SampleLevel(sam, tex_coord, 0).xyz;
}

inline float3 to_large_scene_scale(float3 p) {
	float3 to_p = p - camera.pos.xyz;
	return camera.pos.xyz + normalize(to_p) * (length(to_p) * camera.info.y); // camera.info.y - scale factor for large objects
}

inline float3 to_default_scene_scale(float3 p) {
	float3 to_p = p - camera.pos.xyz;
	return camera.pos.xyz + normalize(to_p) * (length(to_p) / camera.info.y); // camera.info.y - scale factor for large objects
}

inline void TraceRay(in Ray ray, inout TriangleIntersection TriInter) {
	TraceRay(scene, gnb, gvb, ray, TriInter);

	if (input.use_two_scenes == 1u) {
		ray.origin = to_large_scene_scale(ray.origin);

		TriangleIntersection inter = TriInter;
		TraceRay(l_scene, l_gnb, l_gvb, ray, inter);

		if (inter.Distance < TriInter.Distance)
			TriInter = inter;
	}
}

Ray primary_ray(uint2 screen_size, uint2 screen_pixel_pos) {
	const float ct = tan(camera.info.x / 2.0f);
	const float2 screenPos = ((screen_pixel_pos + 0.5f) / (float2)screen_size * 2.0f - 1.0f) * float2(ct * ((float)screen_size.x / screen_size.y), -ct);

	Ray ray;
	ray.origin = camera.pos.xyz;
	ray.dir = mul(float4(normalize(float3(screenPos, -1)), 1.0), camera.view).xyz;

	return ray;
}

void def_scene_rand_triangle_light_dir(inout Ray ray, in uint triangle_id) {
	const float r0 = sqrt(rand_next());
	const float r1 = rand_next();

	ray.dir = normalize((gvb[triangle_id * 3].pos * (1.0f - r0) +
		gvb[triangle_id * 3 + 1].pos * r0 * (1.0f - r1) +
		gvb[triangle_id * 3 + 2].pos * r0 * r1) -
		ray.origin);
}

void def_scene_rand_sphere_light_dir(inout Ray ray, in uint2 sphere_info) { // sphere_info: .x - pos, .y - radius
	// Calculate sphere disk radius
	//float ray_dist = distance(ray.origin, sphere_info.x);
	//float disk_r = distance(ray_dist - sphere_info.y, sqrt(sqr(ray_dist) - sqr(sphere_info.y))) * COS_45DEG;
	//float3 disk_pos = ray.origin + ray.dir * (ray_dist - sphere_info.y + disk_r);

	ray.dir = normalize((rand_cos_hemisphere_dir(normalize(ray.origin - sphere_info.x)) * sphere_info.y) - ray.origin);
}


void large_scene_rand_triangle_light_dir(inout Ray ray, in uint triangle_id) {
	const float r0 = sqrt(rand_next());
	const float r1 = rand_next();

	ray.dir = normalize((l_gvb[triangle_id * 3].pos * (1.0f - r0) +
		l_gvb[triangle_id * 3 + 1].pos * r0 * (1.0f - r1) +
		l_gvb[triangle_id * 3 + 2].pos * r0 * r1) -
		to_large_scene_scale(ray.origin));
}

void large_scene_rand_sphere_light_dir(inout Ray ray, in uint2 sphere_info) { // sphere_info: .x - pos, .y - radius
	// Calculate sphere disk radius
	//float ray_dist = distance(ray.origin, sphere_info.x);
	//float disk_r = distance(ray_dist - sphere_info.y, sqrt(sqr(ray_dist) - sqr(sphere_info.y))) * COS_45DEG;
	//float3 disk_pos = ray.origin + ray.dir * (ray_dist - sphere_info.y + disk_r);

	const float3 pos = to_large_scene_scale(ray.origin);
	ray.dir = normalize((rand_cos_hemisphere_dir(normalize(pos - sphere_info.x)) * sphere_info.y) - pos);
}

void rand_light_dir(inout Ray ray) {
	uint light_index;

	if (rand_next_bool() && input.use_two_scenes) {
		light_index = min(uint(rand_next() * input.large_scene_light_count), input.large_scene_light_count - 1);

	} else {
		light_index = min(uint(rand_next() * input.def_scene_light_count), input.def_scene_light_count - 1);
	}
}

bool trace_shadow_ray(in Ray ray, in uint triangle_id, inout TriangleIntersection inter) {
	inter.Flags = 0;
	TraceRay(ray, inter);
	return inter.TriangleId == triangle_id;
}

void sample_direct(in TriangleIntersection tri_inter, out float4 direct, out float4 direct_albedo) {

	//float NdotL = saturate(dot(inter.normal, toLight));

	//	// Shoot our ray for our direct lighting
	//float shadowMult = trace_shadow_ray(inter.point, testLightPos);

	//// Compute our GGX color
	//vec3 ggxTerm = getGGXColor(toCamera, toLight, inter.normal, NdotV, inter.material.specular, inter.material.roughness, true);

	//// Compute direct color.  Split into light and albedo terms for our SVGF filter
	//vec3 lightIntensity = vec3(1);
	//vec3 directColor = lightIntensity * NdotL * shadowMult;

	////directAlbedo = vec4(ggxTerm + inter.material.emission + inter.material.diffuse / PI, 1);
	//directAlbedo = vec4(ggxTerm + inter.material.emission + inter.material.diffuse / PI, 1);
	//directI = vec4(inter.material.emission + directColor, 1);
}

void sample_indirect(out float4 indirect, out float4 indirect_albedo) {
}

[numthreads(THREAD_GROUP_WIDTH, THREAD_GROUP_HEIGHT, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint2 SIZE;
	rt_output.GetDimensions(SIZE.x, SIZE.y);

	rand_init(DTid.x + DTid.y * SIZE.x, renderer.info.x, 16);

	float4 pos = g_position.SampleLevel(sam, DTid.xy / (float2)SIZE, 0);
	float4 normal = g_normal.SampleLevel(sam, DTid.xy / (float2)SIZE, 0);
	float4 diffuse = g_diffuse.SampleLevel(sam, DTid.xy / (float2)SIZE, 0);
	float4 specular = g_specular.SampleLevel(sam, DTid.xy / (float2)SIZE, 0);
	float4 emission = g_emission.SampleLevel(sam, DTid.xy / (float2)SIZE, 0);

	float4 color = float4(0, 0, 0, renderer.info.x);

	if (pos.w == 1) { // Not a background pixel
		Ray pRay = primary_ray(SIZE, DTid.xy);

		TriangleIntersection tri_inter;
		tri_inter.init(INTERSECTION_FLAG_TEXCOORD);
		TraceRay(pRay, tri_inter);


		if (tri_inter.Intersected) {
			color = diffuse_tex.SampleLevel(sam, float3(tri_inter.TexCoord, 0), 0);
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