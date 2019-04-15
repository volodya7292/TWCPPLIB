Texture2DArray<float4> diffuseTex : register(t0);
Texture2DArray<float4> normalTex : register(t1);
Texture2DArray<float4> roughTex : register(t2);

Texture2D<float4> testTex : register(t3);

sampler sam : register(s0);

struct VS_OUTPUT {
	float4           pos : SV_POSITION;
	float2     tex_coord : TEXCOORD;
	linear float3 normal : OBJECTNORMAL;
};

struct GBuffer {
	float4 position;                    // World space position.  .w component = 0 if a background pixel
	float4 normal;                      // World space normal.  (.w is distance from camera to hit point; this may not be used)
	float4 diffuse;                     // .rgb diffuse material color, .a pixel opacity/transparency
	float4 specular;                    // .rgb Falcor's specular representation, .a specular roughness
	float4 svgfLinZ    : SV_Target4;    // SVGF-specific buffer containing linear z, max z-derivs, last frame's z, obj-space normal
	float4 svgfMoVec   : SV_Target5;    // SVGF-specific buffer containing motion vector and fwidth of pos & normal
	float4 svgfCompact : SV_Target6;    // SVGF-specific buffer containing duplicate data that allows reducing memory traffic in some passes
};

// A simple utility to convert a float to a 2-component octohedral representation packed into one uint
uint dirToOct(float3 normal) {
	float2 p = normal.xy * (1.0 / dot(abs(normal), 1.0.xxx));
	float2 e = normal.z > 0.0 ? p : (1.0 - abs(p.yx)) * (step(0.0, p)*2.0-(float2)(1.0));
	return (asuint(f32tof16(e.y)) << 16) + (asuint(f32tof16(e.x)));
}

// Take current clip position, last frame pixel position and compute a motion vector
float2 calcMotionVector(float4 prevClipPos, float2 currentPixelPos, float2 invFrameSize) {
	float2 prevPosNDC = (prevClipPos.xy / prevClipPos.w) * float2(0.5, -0.5) + float2(0.5, 0.5);
	float2 motionVec  = prevPosNDC - (currentPixelPos * invFrameSize);

	// Guard against inf/nan due to projection by w <= 0.
	const float epsilon = 1e-5f;
	motionVec = (prevClipPos.w < epsilon) ? float2(0, 0) : motionVec;
	return motionVec;
}

float4 main(VS_OUTPUT input) : SV_TARGET
{
	// Grab shading data.  Invert if necessary.
	//ShadingData hitPt = prepareShadingData(vsOut.base, gMaterial, gCamera.posW);

	// Compute data needed for SVGF

	// The 'linearZ' buffer
	float linearZ = input.pos.z * input.pos.w;
    float maxChangeZ = max(abs(ddx(linearZ)), abs(ddy(linearZ)));
//float objNorm    = asfloat(dirToOct(normalize(vsOut.normalObj)));
//float4 svgfLinearZOut = float4(linearZ, maxChangeZ, vsOut.base.prevPosH.z, objNorm);

//// The 'motion vector' buffer
//float2 svgfMotionVec = calcMotionVector(vsOut.base.prevPosH, pos.xy, gBufSize.zw) +
//float2(gCamera.jitterX, -gCamera.jitterY);
//float2 posNormFWidth = float2(length(fwidth(hitPt.posW)), length(fwidth(hitPt.N)));
//float4 svgfMotionVecOut = float4(svgfMotionVec, posNormFWidth);

//// Dump out our G buffer channels
//GBuffer gBufOut;
//gBufOut.wsPos     = float4(hitPt.posW, 1.f);
//gBufOut.wsNorm    = float4(hitPt.N, length(hitPt.posW - gCamera.posW));
//gBufOut.matDif    = float4(hitPt.diffuse, hitPt.opacity);
//gBufOut.matSpec   = float4(hitPt.specular, hitPt.linearRoughness);
//gBufOut.svgfLinZ  = svgfLinearZOut;
//gBufOut.svgfMoVec = svgfMotionVecOut;

//// A compacted buffer containing discretizied normal, depth, depth derivative
//gBufOut.svgfCompact = float4(asfloat(dirToOct(hitPt.N)), linearZ, maxChangeZ, 0.0f);

//return gBufOut;





// return interpolated color

//GBuffer g;
/*g.out0 = lerp(tex[0].Sample(s1, input.texCoord), tex[1].Sample(texs, input.texCoord), 0.5);
g.out1 = float4(1, 1, 0, 1);

g.out0 = buffer[input.pos.x].o * lerp(OutputTexture[input.pos.xy], g.out0, 0.3);*/

return diffuseTex.Sample(sam, float3(input.tex_coord, 0));
//return lerp(t1.Sample(s1, input.texCoord), tex.Sample(texs, input.texCoord), 0.5);
}