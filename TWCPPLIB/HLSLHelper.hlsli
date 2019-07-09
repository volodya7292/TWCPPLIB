#define THREAD_GROUP_WIDTH 8
#define THREAD_GROUP_HEIGHT 8
#define THREAD_GROUP_1D_WIDTH (THREAD_GROUP_WIDTH * THREAD_GROUP_HEIGHT)

#define        FLT_MAX    3.402823466e+38f
#define MachineEpsilon    5.96e-08
#define             PI    3.141592653
#define            PI2    6.283185307

#define sqr(x) (x) * (x)

#define LIGHTSOURCE_TYPE_TRIANGLE    0
#define   LIGHTSOURCE_TYPE_SPHERE    1

static uint rand_seed;

float floor(float v, float unit_size) {
	return floor(v / unit_size) * unit_size;
}

float2 floor(float2 v, float2 unit_size) {
	return floor(v / unit_size) * unit_size;
}

float4x4 scale(float4x4 m, in float v) {
	m[0][0] *= v; m[1][0] *= v; m[2][0] *= v;
	m[0][1] *= v; m[1][1] *= v; m[2][1] *= v;
	m[0][2] *= v; m[1][2] *= v; m[2][2] *= v;
	m[0][3] *= v; m[1][3] *= v; m[2][3] *= v;
	return m;
}

float4x4 translate(float4x4 m, in float3 v) {
	m[0][3] = v.x;
	m[1][3] = v.y;
	m[2][3] = v.z;
	return m;
}

inline float3 translation(in float4x4 m) {
	return float3(m[0][3], m[1][3], m[2][3]);
}

inline float depth_linearize(float d, float zNear, float zFar) {
	return zNear * zFar / (zFar + d * (zNear - zFar));
}

inline float depth_delinearize(float ld, float zNear, float zFar) {
	return (zNear * zFar / ld - zFar) / (zNear - zFar);
}

struct VSQuadOutput {
	float4 pos       : SV_Position;
	float2 tex_coord : TEXCOORD;
};

struct Vertex {
	float3          pos : POSITION;
	float3    tex_coord : TEXCOORD; // .z - material id
	float3       normal : NORMAL;
	float3      tangent : TANGENT;
};

struct Ray {
	float3 origin;
	float3 dir;
};

struct Bounds {
	float4 pMin;
	float4 pMax;
	inline static float gamma(int n) {
		return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
	}

	inline float4 i(int i) {
		return (i == 0) ? pMin : pMax;
	}
	bool intersect(Ray ray) {
		//distan = FLT_MAX;

		const float3 invD = rcp(ray.dir);
		const float3 t0 = (pMin.xyz - ray.origin) * invD;
		const float3 t1 = (pMax.xyz - ray.origin) * invD;

		const float3 tsmaller = min(t0, t1);
		const float3 tbigger  = max(t0, t1);

		//const float tmin = max(RAY_TMIN, max(tsmaller[0], max(tsmaller[1], tsmaller[2])));
		//const float tmax = min(RAY_TMAX, min(tbigger[0], min(tbigger[1], tbigger[2])));
		const float tmin = max(tsmaller[0], max(tsmaller[1], tsmaller[2]));
		const float tmax = min(tbigger[0], min(tbigger[1], tbigger[2]));

		//if (tmin < distan) distan = t0;
		//if (tmax < distan) distan = t1;

		return (tmax >= tmin);
	}
};

struct LBVHNode {
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

struct SceneLBVHNode {
	SceneLBVHInstance instance;
	Bounds bounds;
	uint parent;
	uint left_child;
	uint right_child;
};

struct Material {
	uint4 texture_ids0; // .x - diffuse, .y - specular, .z - normal
	uint4 texture_ids1; // .x - diffuse, .y - specular, .z - normal
	// if texture_id == -1 then the parameters below are used
	float4 diffuse;  // .a - opacity/translucency
	float4 specular; // .a - roughness
	float4 emission; // .a - texture lerp blend factor
};

struct LightSource {
	float4 v[3];      // v[0] - sphere position; v[0], v[1], v[2] - triangle vertices
	float4 normal;    // .xyz - triangle normal, .w - triangle material id
	float4 info;      // .x - type, .y - sphere radius
};

struct Camera {
	float4 pos;
	float4x4 proj;
	float4x4 view;
	float4x4 proj_view;
	float4 info; // .x - FOVy in radians
};

struct RendererInfo {
	uint4 info; // .x - width, .y - height, .z - frame index, .w - scale factor for large objects
};

typedef StructuredBuffer<SceneLBVHNode> RTScene;
typedef StructuredBuffer<LBVHNode>      RTNB;   // Ray Tracing Node Buffer
typedef StructuredBuffer<Vertex>        GVB;    // Global Vertex Buffer
typedef StructuredBuffer<LightSource>   LSB;    // Light Source Buffer
typedef StructuredBuffer<Material>      MaterialBuffer;


// -----------------------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------------------------

inline static float3 swizzle(float3 v, int3 swizzleOrder) {
	return float3(v[swizzleOrder.x], v[swizzleOrder.y], v[swizzleOrder.z]);
}

inline static uint max_dim(float3 v) {
	float3 v2 = abs(v);

	if (v2.x > v2.y) {
		// y isn't the maximum, so it's either x or z
		if (v2.x > v2.z) {
			return 0;
		} else {
			return 2;
		}
	} else if (v2.y > v2.z) {
		// x isn't the maximum, so it's either y or z
		return 1;
	} else {
		return 2;
	}
}

inline float3 Swizzle(float3 v, int3 swizzleOrder) {
	return float3(v[swizzleOrder.x], v[swizzleOrder.y], v[swizzleOrder.z]);
}

inline bool IsPositive(float f) { return f > 0.0f; }

inline float luminance(float3 rgb) {
	return dot(rgb, float3(0.2126f, 0.7152f, 0.0722f));
}

void rand_init(uint val0, uint val1, uint backoff = 16) {
	uint v0 = val0, v1 = val1, s0 = 0;

	for (uint n = 0; n < backoff; n++) {
		s0 += 0x9e3779b9;
		v0 += ((v1 << 4) + 0xa341316c) ^ (v1 + s0) ^ ((v1 >> 5) + 0xc8013ea4);
		v1 += ((v0 << 4) + 0xad90777d) ^ (v0 + s0) ^ ((v0 >> 5) + 0x7e95761e);
	}

	rand_seed = v0;
}

// Random range: [0;1]
inline float rand_next() {
	rand_seed = 1664525u * rand_seed + 1013904223u;
	return float(rand_seed & 0x00FFFFFF) / float(0x01000000);
}

inline bool rand_next_bool() {
	return round(rand_next());
}

float3 get_perpendicular_vector(float3 u) {
	float3 a = abs(u);
	uint xm = ((a.x - a.y) < 0 && (a.x - a.z) < 0) ? 1 : 0;
	uint ym = (a.y - a.z) < 0 ? (1 ^ xm) : 0;
	uint zm = 1 ^ (xm | ym);
	return cross(u, float3(xm, ym, zm));
}

// Get a cosine-weighted random vector centered around a specified normal direction.
float3 rand_cos_hemisphere_dir(float3 normal) {
	// Get 2 random numbers to select our sample with
	float2 randVal = float2(rand_next(), rand_next());

	// Cosine weighted hemisphere sample from RNG
	float3 bitangent = get_perpendicular_vector(normal);
	float3 tangent = cross(bitangent, normal);
	float r = sqrt(randVal.x);
	float phi = PI2 * randVal.y;

	// Get our cosine-weighted hemisphere lobe sample direction
	return tangent * (r * cos(phi).x) + bitangent * (r * sin(phi)) + normal.xyz * sqrt(max(0.0, 1.0f - randVal.x));
}

inline float ggx_normal_distribution(float NdotH, float roughness) {
	float a2 = roughness * roughness;
	float d = ((NdotH * a2 - NdotH) * NdotH + 1);
	return a2 / max(0.001f, (d * d * PI));
}

inline float ggx_smith_masking_term(float NdotL, float NdotV, float roughness) {
	float a2 = roughness * roughness;
	float lambdaV = NdotL * sqrt(max(0.0f, (-NdotV * a2 + NdotV) * NdotV + a2));
	float lambdaL = NdotV * sqrt(max(0.0f, (-NdotL * a2 + NdotL) * NdotL + a2));
	return 0.5f / max(0.001f, (lambdaV + lambdaL));
}

inline float3 schlick_fresnel(float3 f0, float u) {
	return f0 + (float3(1.0f, 1.0f, 1.0f) - f0) * pow(1.0f - u, 5.0f);
}

// Compute GGX term from input parameters
float3 get_direct_ggx_color(float3 V, float3 L, float3 N, float NdotV, float3 specColor, float roughness) {
	// Compute half vector and dot products
	float3 H = normalize(V + L);
	float NdotL = saturate(dot(N, L));
	float NdotH = saturate(dot(N, H));
	float LdotH = saturate(dot(L, H));

	// Evaluate our GGX BRDF term
	float D = ggx_normal_distribution(NdotH, roughness);
	float G = ggx_smith_masking_term(NdotL, NdotV, roughness) * 4 * NdotL;
	float3 F = schlick_fresnel(specColor, LdotH);

	// Determine if the color is valid (if invalid, we likely have a NaN or Inf)
	return (NdotV * NdotL * LdotH <= 0.0f) ? 0 : (F * G * D * NdotV);
}

// Compute GGX term from input parameters
float3 get_indirect_ggx_color(float3 V, float3 L, float3 N, float NdotV, float3 specColor, float roughness) {
	// Compute half vector and dot products
	float3 H = normalize(V + L);
	float NdotL = saturate(dot(N, L));
	float NdotH = saturate(dot(N, H));
	float LdotH = saturate(dot(L, H));

	// Evaluate our GGX BRDF term
	float G = ggx_smith_masking_term(NdotL, NdotV, roughness) * 4 * NdotL;
	float3 F = schlick_fresnel(specColor, LdotH);

	// Determine if the color is valid (if invalid, we likely have a NaN or Inf)
	return (NdotV * NdotL * LdotH <= 0.0f) ? 0 : (F * G * LdotH / max(1e-3f, NdotH));
}

float probability_to_sample_diffuse(float3 diffuse, float3 specular) {
	float lumDiffuse = luminance(diffuse);
	float lumSpecular = luminance(specular);
	return lumDiffuse / max(1e-3f, lumDiffuse + lumSpecular);
}

float3 rand_ggx_sample_dir(float roughness, float3 normal, float3 inVec) {
	// Get our uniform random numbers
	const float2 randVal = float2(rand_next(), rand_next());

	// Get an orthonormal basis from the normal
	const float3 B = get_perpendicular_vector(normal);
	const float3 T = cross(B, normal);

	// GGX NDF sampling
	const float a2 = roughness * roughness;
	const float cosThetaH = sqrt(max(0.0f, (1.0 - randVal.x) / ((a2 - 1.0) * randVal.x + 1)));
	const float sinThetaH = sqrt(max(0.0f, 1.0f - cosThetaH * cosThetaH));
	const float phiH = randVal.y * PI * 2.0f;

	// Get our GGX NDF sample (i.e., the half vector)
	const float3 H = T * (sinThetaH * cos(phiH)) + B * (sinThetaH * sin(phiH)) + normal * cosThetaH;

	// Convert this into a ray direction by computing the reflection direction
	return normalize(2.f * dot(inVec, H) * H - inVec);
}

uint4 pack_f2_16(float4 v0, float4 v1) {
	// 1.0 -> 65535
	return (f32tof16(v1) << 16) | f32tof16(v0);
}

void unpack_f2_16(uint4 packed, out float4 v0, out float4 v1) {
	// 65535 -> 1.0
	v0 = f16tof32(packed & 0x0000FFFF);
	v1 = f16tof32((packed >> 16) & 0x0000FFFF);
}

inline float dot180(float3 v0, float3 v1) {
	return (1 + dot(v0, v1)) * 0.5f;
}

inline float triangle_area(float3 v0, float3 v1, float3 v2) {
	const float3 c = cross(v1 - v0, v2 - v0);
	return 0.5f * sqrt(sqr(c.x) + sqr(c.y) + sqr(c.z));
}

inline float hemisphere_area(float r) {
	return 2 * PI * sqr(r);
}

inline float triangle_light_spread(float3 p, float3 v0, float3 v1, float3 v2) {
	const float3 c = normalize((v0 + v1 + v2) / 3.0f - p);
	return (dot180(normalize(v0 - p), c) + dot180(normalize(v1 - p), c) + dot180(normalize(v2 - p), c)) / 3.0f;
}

inline float sphere_light_spread(float3 p, float3 v0, float r) {
	const float3 v = get_perpendicular_vector(normalize(p - v0));
	return 1 - dot180(normalize(v0 - v * r - p), normalize(v0 + v * r - p));
}