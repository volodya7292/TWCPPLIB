Texture2DArray<float4> diffuseTex : register(t0);
Texture2DArray<float4> normalTex : register(t1);
Texture2DArray<float4> roughTex : register(t2);

sampler sam : register(s0);

struct VS_OUTPUT {
	float4 pos: SV_POSITION;
	float2 texCoord: TEXCOORD;
};

//struct GBuffer {
//	float4 out0;
//	float4 out1;
//};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	// return interpolated color

	//GBuffer g;
	/*g.out0 = lerp(tex[0].Sample(s1, input.texCoord), tex[1].Sample(texs, input.texCoord), 0.5);
	g.out1 = float4(1, 1, 0, 1);

	g.out0 = buffer[input.pos.x].o * lerp(OutputTexture[input.pos.xy], g.out0, 0.3);*/

	return float4(1, 0.3, 1, 0);
	//return lerp(t1.Sample(s1, input.texCoord), tex.Sample(texs, input.texCoord), 0.5);
}