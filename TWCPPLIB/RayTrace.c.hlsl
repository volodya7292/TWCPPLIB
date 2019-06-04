#include "RayTracing.hlsli"

#define COS_45DEG 0.7071067811

#define RT_SHADOW_BIAS    1e-2f

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

struct LightInfo {
	float ray_length;
	float3 color;
};


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

		if (inter.Distance < TriInter.Distance) {
			inter.Point = to_default_scene_scale(inter.Point);
			//inter.Distance = distance(ray.origin, inter.Point);
			TriInter = inter;
		}
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

inline void def_scene_rand_triangle_light_dir(inout Ray ray, in uint triangle_id, out float ray_length) {
	const float r0 = sqrt(rand_next());
	const float r1 = rand_next();

	const float3 tpos =
		gvb[triangle_id * 3].pos * (1.0f - r0) +
		gvb[triangle_id * 3 + 1].pos * r0 * (1.0f - r1) +
		gvb[triangle_id * 3 + 2].pos * r0 * r1;

	ray_length = distance(ray.origin, tpos);
	ray.dir = normalize(tpos - ray.origin);
}

inline void def_scene_rand_sphere_light_dir(inout Ray ray, in LightSource sphere_light, out float ray_length) {
	// Calculate sphere disk radius
	//float ray_dist = distance(ray.origin, sphere_info.x);
	//float disk_r = distance(ray_dist - sphere_info.y, sqrt(sqr(ray_dist) - sqr(sphere_info.y))) * COS_45DEG;
	//float3 disk_pos = ray.origin + ray.dir * (ray_dist - sphere_info.y + disk_r);

	const float3 spos = sphere_light.pos.xyz + (rand_cos_hemisphere_dir(normalize(ray.origin - sphere_light.pos.xyz)) * sphere_light.info.z);

	ray_length = distance(ray.origin, spos);
	ray.dir = normalize(spos - ray.origin);
}


inline void large_scene_rand_triangle_light_dir(inout Ray ray, in uint triangle_id, out float ray_length) {
	const float r0 = sqrt(rand_next());
	const float r1 = rand_next();

	const float3 tpos =
		l_gvb[triangle_id * 3].pos * (1.0f - r0) +
		l_gvb[triangle_id * 3 + 1].pos * r0 * (1.0f - r1) +
		l_gvb[triangle_id * 3 + 2].pos * r0 * r1;

	const float3 ro = to_large_scene_scale(ray.origin);
	ray_length = distance(ro, tpos);
	ray.dir = normalize(tpos - ro);
}

inline void large_scene_rand_sphere_light_dir(inout Ray ray, in LightSource sphere_light, out float ray_length) {
	const float3 ro = to_large_scene_scale(ray.origin);
	const float3 spos = sphere_light.pos.xyz + (rand_cos_hemisphere_dir(normalize(ro - sphere_light.pos.xyz)) * sphere_light.info.z);
	ray_length = distance(ro, spos);
	ray.dir = normalize(spos - ro);
}

void rand_light_dir(inout Ray ray, out LightInfo light_info) {
	uint light_index;

	LightSource light;

	light_info.ray_length = 0;
	ray.dir = 0;

	if (rand_next_bool() && input.use_two_scenes) {
		light_index = min(uint(rand_next() * input.large_scene_light_count), input.large_scene_light_count - 1);
		light = l_lsb[light_index];

		if (light.info.x == LIGHTSOURCE_TYPE_TRIANGLE)
			large_scene_rand_triangle_light_dir(ray, light.info.w + light.info.y, light_info.ray_length);
		else if (light.info.x == LIGHTSOURCE_TYPE_SPHERE)
			large_scene_rand_sphere_light_dir(ray, light, light_info.ray_length);
	} else {
		light_index = min(uint(rand_next() * input.def_scene_light_count), input.def_scene_light_count - 1);
		light = lsb[light_index];

		if (light.info.x == LIGHTSOURCE_TYPE_TRIANGLE)
			def_scene_rand_triangle_light_dir(ray, light.info.w + light.info.y, light_info.ray_length);
		else if (light.info.x == LIGHTSOURCE_TYPE_SPHERE)
			def_scene_rand_sphere_light_dir(ray, light, light_info.ray_length);
	}

	light_info.color = light.color.rgb;
}

// 1 - light visible, 0 - light invisible
inline bool trace_shadow_ray(in Ray ray, in float light_ray_length) {
	TriangleIntersection inter;
	inter.init(0);

	if (input.def_scene_light_count > 0 || input.large_scene_light_count > 0) {
		TraceRay(ray, inter);
		return (inter.Distance >= light_ray_length - RT_SHADOW_BIAS) || (abs(inter.Distance - light_ray_length) <= RT_SHADOW_BIAS);
	} else {
		return false;
	}
}

float3 trace_indirect_ray(in Ray ray) {
	TriangleIntersection inter;
	inter.init(INTERSECTION_FLAG_TEXCOORD | INTERSECTION_FLAG_NORMAL);

	TraceRay(ray, inter);


	LightInfo light_info;
	Ray to_light;
	to_light.origin = ray.origin;
	rand_light_dir(to_light, light_info);


	// Compute our lambertion term (L dot N)
	//vec3 toLight = normalize(inter.point - ray.origin);
	float LdotN = saturate(dot(inter.Normal, to_light.dir));

	// Shoot our shadow ray.
	//Intersection shadow = trace_ray(ray);
	float shadowMult = trace_shadow_ray(to_light, light_info.ray_length);

	float4 diffuse = diffuse_tex.SampleLevel(sam, inter.TexCoord, 0);
	float4 emission = emission_tex.SampleLevel(sam, inter.TexCoord, 0);

	return emission.xyz + LdotN * shadowMult * light_info.color * (diffuse.xyz) / PI;
	//return diffuse.xyz;
}

float4 sample_color(in float3 pos, in float3 normal, in float3 diffuse, in float3 specular, in float3 emission, in float roughness) {
	float4 direct, direct_albedo;
	float4 indirect, indirect_albedo;


	float3 to_camera = normalize(camera.pos.xyz - pos);

	/*float randX = rand_next() * 1 - 0.5;
	float randZ = rand_next() * 1 - 0.5;
	float3 testLightPos = (float3(0, 0.5f, 0) + float3(randX, 0, randZ));
	float3 toLight = normalize(testLightPos - pos);*/


	LightInfo light_info;
	Ray to_light;
	to_light.origin = pos;
	//to_light.dir = toLight;//normalize(float3(0, 0.49f, 0) - pos);
	//light_info.color = float3(1, 1, 1);
	//light_info.ray_length = 0;
	rand_light_dir(to_light, light_info);

	float NdotL = saturate(dot(normal, to_light.dir));
	float NdotV = dot(normal, to_camera);
	float shadowMult = trace_shadow_ray(to_light, light_info.ray_length);

	// Direct
	{
		// Compute our GGX color
		float3 ggxTerm = get_ggx_color(to_camera, to_light.dir, normal, NdotV, specular, roughness, true);

		// Compute direct color.  Split into light and albedo terms for our SVGF filter
		float3 directColor = emission + shadowMult * light_info.color * NdotL;//(normal + 1.0f) / 2.0f;//shadowMult * NdotL;
		float3 directAlbedo = emission + ggxTerm + diffuse / PI;
		//bool colorsNan = any(isnan(directColor)) || any(isnan(directAlbedo));
		direct = float4(saturate(directColor), 1);//float4(colorsNan ? float3(0, 0, 0) : directColor, 1.0f);
		direct_albedo = float4(saturate(directAlbedo), 1);//float4(colorsNan ? float3(0, 0, 0) : directAlbedo, 1.0f);
		//direct = float4(emission + NdotL, 1);
		//direct_albedo = float4(diffuse, 1);
	}

	// Indirect
	{
		// We have to decide whether we sample our diffuse or specular lobe.
		float probDiffuse   = probability_to_sample_diffuse(diffuse, specular);
		float chooseDiffuse = (rand_next() < probDiffuse);

		float3 bounceDir;
		if (chooseDiffuse) {   // Randomly select to bounce in our diffuse lobe
			bounceDir = rand_cos_hemisphere_dir(normal);
		} else {   // Randomyl select to bounce in our GGX lobe
			bounceDir = rand_ggx_sample_dir(roughness, normal, to_camera);
		}

		// Shoot our indirect color ray
		Ray iRay;
		iRay.origin = pos;
		iRay.dir = bounceDir;
		float3 bounceColor = trace_indirect_ray(iRay);

		// Compute diffuse, ggx shading terms
		float  NdotL = saturate(dot(pos, bounceDir));
		float3 difTerm = max(5e-3f, diffuse / PI);
		float3 ggxTerm = NdotL * get_ggx_color(to_camera, bounceDir, normal, NdotV, specular, roughness, false);

		// Split into an incoming light and "indirect albedo" term to help filter illumination despite sampling 2 different lobes
		float3 difFinal = 1.0f / probDiffuse;                    // Has been divided by difTerm.  Multiplied back post-SVGF
		float3 ggxFinal = ggxTerm / (difTerm * (1.0f - probDiffuse));    // Has been divided by difTerm.  Multiplied back post-SVGF
		float3 shadeColor = bounceColor * (chooseDiffuse ? difFinal : ggxFinal);

		bool colorsNan = any(isnan(shadeColor));
		indirect = float4(colorsNan ? float3(0, 0, 0) : shadeColor, 1.0f);
		indirect_albedo = float4(difTerm, 1.0f);
	}

	return direct * direct_albedo + indirect * indirect_albedo;
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

		//pRay.dir = float3(1, 0, 0);

		/*TriangleIntersection tri_inter;
		tri_inter.init(INTERSECTION_FLAG_NORMAL);
		TraceRay(pRay, tri_inter);*/

		color = sample_color(pos.xyz, normal.xyz, diffuse.rgb, specular.rgb, emission.rgb, 1.0f);

		//if (tri_inter.Intersected)
		//	color = diffuse_tex.SampleLevel(sam, float3(tri_inter.TexCoord, 0), 0);

	} else { // Background pixel
		float4 e1 = diffuse_tex.SampleLevel(sam, float3(DTid.xy / (float2)SIZE, 0), 0);
		float4 e = emission_tex.SampleLevel(sam, float3(DTid.xy / (float2)SIZE, 0), 0);
		float4 e2 = normal_tex.SampleLevel(sam, float3(DTid.xy / (float2)SIZE, 0), 0);
		float4 e3 = specular_tex.SampleLevel(sam, float3(DTid.xy / (float2)SIZE * e.x * e2.x, 0), 0);
		color = float4(normal.x, diffuse.y, specular.z, emission.a) * lsb[0].info * l_lsb[0].info * e * e2 * e3 * e1 * renderer.info.x;
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