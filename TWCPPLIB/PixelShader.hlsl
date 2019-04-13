//Texture2D<float4> t1 : register(t0);
//Texture2D<float4> tex : register(t2);
Texture2D<float4> tex[2] : register(t0);
RWTexture2D<float4> OutputTexture : register(u0);
sampler s1 : register(s0);
sampler texs : register(s1);

struct VS_OUTPUT
{
    float4 pos: SV_POSITION;
    float2 texCoord: TEXCOORD;
};

struct GBuffer {
	float4 out0;
	float4 out1;
};

GBuffer main(VS_OUTPUT input) : SV_TARGET
{
    // return interpolated color

	GBuffer g;
	g.out0 = lerp(tex[0].Sample(s1, input.texCoord), tex[1].Sample(texs, input.texCoord), 0.5);
	g.out1 = float4(1, 1, 0, 1);

	g.out0 = lerp(OutputTexture[input.pos.xy], g.out0, 0.3);

	return g;
    //return lerp(t1.Sample(s1, input.texCoord), tex.Sample(texs, input.texCoord), 0.5);
}