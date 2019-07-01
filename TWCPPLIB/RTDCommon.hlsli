// A simple utility to convert a float to a 2-component octohedral representation packed into one uint
uint dir_to_oct(float3 normal) {
	float2 p = normal.xy * (1.0 / dot(abs(normal), 1.0.xxx));
	float2 e = normal.z > 0.0 ? p : (1.0 - abs(p.yx)) * (step(0.0, p) * 2.0 - (float2)(1.0));
	return (asuint(f32tof16(e.y)) << 16) + (asuint(f32tof16(e.x)));
}

float3 oct_to_dir(uint octo) {
	float2 e = float2(f16tof32(octo & 0xFFFF), f16tof32((octo >> 16) & 0xFFFF));
	float3 v = float3(e, 1.0 - abs(e.x) - abs(e.y));
	if (v.z < 0.0)
		v.xy = (1.0 - abs(v.yx)) * (step(0.0, v.xy) * 2.0 - (float2)(1.0));
	return normalize(v);
}

void fetch_normal_and_linear_z(Texture2D tex, uint2 pixel, out float3 normal, out float2 linear_z) {
	float4 nd = tex[pixel];
	normal = normalize(oct_to_dir(asuint(nd.x)));
	linear_z = float2(nd.y, nd.z);
}

void fetch_normal_and_linear_z(Texture2D tex, uint2 pixel, out float3 normal, out float2 linear_z, out float roughness) {
	float4 nd = tex[pixel];
	normal = normalize(oct_to_dir(asuint(nd.x)));
	linear_z = float2(nd.y, nd.z);
	roughness = nd.w;
}

float2 computeWeight(
	float depthCenter, float depthP, float phiDepth,
	float3 normalCenter, float3 normalP,
	float luminanceDirectCenter, float luminanceDirectP,
	float luminanceIndirectCenter, float luminanceIndirectP) {

	const float wNormal    = pow(max(0.0, dot(normalCenter, normalP)), 128.0);
	const float wZ         = (phiDepth == 0) ? 0.0f : abs(depthCenter - depthP) / phiDepth;
	const float wLdirect   = abs(luminanceDirectCenter - luminanceDirectP) / 10.0f;
	const float wLindirect = abs(luminanceIndirectCenter - luminanceIndirectP) / 10.0f;

	const float wDirect   = exp(0.0 - max(wLdirect, 0.0)   - max(wZ, 0.0)) * wNormal;
	const float wIndirect = exp(0.0 - max(wLindirect, 0.0) - max(wZ, 0.0)) * wNormal;

	return float2(wDirect, wIndirect);
}

float2 computeWeight(
	float depthCenter, float depthP, float phiDepth,
	float3 normalCenter, float3 normalP,
	float luminanceDirectCenter, float luminanceDirectP, float phiDirect,
	float luminanceIndirectCenter, float luminanceIndirectP, float phiIndirect) {

	const float wNormal    = pow(max(0.0, dot(normalCenter, normalP)), 128.0);
	const float wZ         = (phiDepth == 0) ? 0.0f : abs(depthCenter - depthP) / phiDepth;
	const float wLdirect   = abs(luminanceDirectCenter - luminanceDirectP) / phiDirect;
	const float wLindirect = abs(luminanceIndirectCenter - luminanceIndirectP) / phiIndirect;

	const float wDirect   = exp(0.0 - max(wLdirect, 0.0)   - max(wZ, 0.0)) * wNormal;
	const float wIndirect = exp(0.0 - max(wLindirect, 0.0) - max(wZ, 0.0)) * wNormal;

	return float2(wDirect, wIndirect);
}