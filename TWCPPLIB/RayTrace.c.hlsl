#include "RayTracing.hlsli"

#define COS_45DEG 0.7071067811

#define RT_SHADOW_BIAS    1e-2f

struct InputData {
	uint use_two_scenes;
	uint def_scene_light_count;
	uint large_scene_light_count;
};

// Default scale scene
GVB       gvb;
RTScene scene;
RTNB      gnb;
LSB       lsb;

// Large scale scene
GVB       l_gvb;
RTScene l_scene;
RTNB      l_gnb;
LSB       l_lsb;


// Texture resources
Texture2DArray<float4> diffuse_tex;
//Texture2DArray<float4> specular_tex;
Texture2DArray<float4> emission_tex;
Texture2DArray<float4> normal_tex;

// GBuffer resources
Texture2D<float4> g_position;
Texture2D<float4> g_normal;
Texture2D<float4> g_diffuse;
Texture2D<float4> g_specular;
Texture2D<float4> g_emission;
sampler sam : register(s0);

// Output image
RWTexture2D<float4> rt_direct;
RWTexture2D<float4> rt_direct_albedo;
RWTexture2D<float4> rt_indirect;
RWTexture2D<float4> rt_indirect_albedo;

// Camera
ConstantBuffer<Camera> camera;
// Scene data
ConstantBuffer<InputData> input;
ConstantBuffer<RendererInfo> renderer;

struct LightInfo {
	float ray_length;
	float3 color;
};


inline void load_texture_data(float3 tex_coord, out float4 diffuse, out float4 emission, out float3 normal) {
	diffuse = diffuse_tex.SampleLevel(sam, tex_coord, 0);
	//specular = specular_tex.SampleLevel(sam, tex_coord, 0);
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

Ray primary_ray(uint2 screen_size, float2 screen_pixel_pos) {
	const float ct = tan(camera.info.x / 2.0f);
	const float2 screenPos = (screen_pixel_pos / (float2)screen_size * 2.0f - 1.0f) * float2(ct * ((float)screen_size.x / screen_size.y), -ct);

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

inline void rand_light_dir(inout Ray ray, out LightInfo light_info) {
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

inline float3 trace_indirect_ray(in Ray ray) {
	TriangleIntersection inter;
	inter.init(INTERSECTION_FLAG_TEXCOORD | INTERSECTION_FLAG_NORMAL);

	TraceRay(ray, inter);


	LightInfo light_info;
	Ray to_light;
	to_light.origin = ray.origin;
	rand_light_dir(to_light, light_info);

	float4 diffuse, emission;
	float3 normal;
	load_texture_data(inter.TexCoord, diffuse, emission, normal);

	// Compute our lambertion term (L dot N)
	//vec3 toLight = normalize(inter.point - ray.origin);
	const float LdotN = saturate(dot(inter.Normal * normal, to_light.dir));

	// Shoot our shadow ray.
	//Intersection shadow = trace_ray(ray);
	const float shadowMult = trace_shadow_ray(to_light, light_info.ray_length);

	return emission.xyz + LdotN * shadowMult * light_info.color * (diffuse.xyz) / PI;
	//return diffuse.xyz;
}

inline void sample_color(in float3 pos, in float3 normal, in float3 diffuse, in float3 specular, in float3 emission, in float roughness, in float2 rt_pixel_pos) {

	float4 direct, direct_albedo, indirect, indirect_albedo;

	const float3 to_camera = normalize(camera.pos.xyz - pos);

	LightInfo light_info;
	Ray to_light;
	to_light.origin = pos;
	rand_light_dir(to_light, light_info);

	const float NdotV = dot(normal, to_camera);
	const float shadowMult = trace_shadow_ray(to_light, light_info.ray_length);

	float3 ggxTerm;
	float NdotL;

	// Direct
	{
		NdotL = saturate(dot(normal, to_light.dir));

		// Compute our GGX color
		ggxTerm = get_ggx_color(to_camera, to_light.dir, normal, NdotV, specular, roughness, true);

		// Compute direct color.  Split into light and albedo terms for our SVGF filter
		const float3 directColor = (equals(emission, 0) ? 0 : 1) + shadowMult * light_info.color * NdotL;//(normal + 1.0f) / 2.0f;//shadowMult * NdotL;
		const float3 directAlbedo = emission + ggxTerm + diffuse / PI;
		bool colorsNan = any(isnan(directColor)) || any(isnan(directAlbedo));
		direct = float4(colorsNan ? float3(0, 0, 0) : directColor, 1.0f);
		direct_albedo = float4(colorsNan ? float3(0, 0, 0) : directAlbedo, 1.0f);
	}

	// Indirect
	{
		// We have to decide whether we sample our diffuse or specular lobe.
		const float probDiffuse   = probability_to_sample_diffuse(diffuse, specular);
		const float chooseDiffuse = (rand_next() < probDiffuse);

		float3 bounceDir;
		
		if (chooseDiffuse) {   // Randomly select to bounce in our diffuse lobe
			bounceDir = rand_cos_hemisphere_dir(normal);
		} else {   // Randomly select to bounce in our GGX lobe
			bounceDir = rand_ggx_sample_dir(roughness, normal, to_camera);
		}

		// Shoot our indirect color ray
		Ray iRay;
		iRay.origin = pos;
		iRay.dir = bounceDir;
		const float3 bounceColor = trace_indirect_ray(iRay);

		// Compute diffuse, ggx shading terms
		NdotL = saturate(dot(normal, bounceDir));
		const float3 difTerm = max(5e-3f, diffuse / PI);
		ggxTerm = NdotL * get_ggx_color(to_camera, bounceDir, normal, NdotV, specular, roughness, false);

		// Split into an incoming light and "indirect albedo" term to help filter illumination despite sampling 2 different lobes
		const float3 difFinal = 1.0f / probDiffuse;                    // Has been divided by difTerm.  Multiplied back post-SVGF
		const float3 ggxFinal = ggxTerm / (difTerm * (1.0f - probDiffuse));    // Has been divided by difTerm.  Multiplied back post-SVGF
		const float3 shadeColor = bounceColor * (chooseDiffuse ? difFinal : ggxFinal);

		const bool colorsNan = any(isnan(shadeColor));
		indirect = float4(colorsNan ? float3(0, 0, 0) : shadeColor, 1.0f);
		indirect_albedo = float4(difTerm, 1.0f);
	}

	rt_direct[rt_pixel_pos] = direct;
	rt_direct_albedo[rt_pixel_pos] = direct_albedo;
	rt_indirect[rt_pixel_pos] = indirect;
	rt_indirect_albedo[rt_pixel_pos] = indirect_albedo;
}

[numthreads(THREAD_GROUP_WIDTH, THREAD_GROUP_HEIGHT, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	uint2 RT_SIZE, G_SIZE;
	rt_direct.GetDimensions(RT_SIZE.x, RT_SIZE.y);
	g_position.GetDimensions(G_SIZE.x, G_SIZE.y);

	const float2 G_SCALE = G_SIZE / float2(RT_SIZE);


	rand_init(DTid.x + DTid.y * RT_SIZE.x, renderer.info.z, 16);

	const uint2 g_pixel = DTid.xy * G_SCALE;
	const uint2 rt_pixel = DTid.xy;

	float4 pos = g_position[g_pixel];
	float4 normal, diffuse, specular, emission;

	if (pos.w == 1) { // Not a background pixel
		normal = g_normal[g_pixel];
		diffuse = g_diffuse[g_pixel];
		specular = g_specular[g_pixel];
		emission = g_emission[g_pixel];

		sample_color(pos.xyz, normal.xyz, diffuse.rgb, 0, emission.rgb, clamp(specular.a + 1, 0, 1), rt_pixel);

	} else { // Background pixel
		rt_direct[rt_pixel] = float4(0, 0, 0, 1) ;
		rt_indirect[rt_pixel] = float4(0, 0, 0, 1);
	}
}