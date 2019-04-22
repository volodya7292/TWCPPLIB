Texture2D<float4> tex : register(t0);
sampler texSam : register(s0);

struct VS_OUTPUT {
	float4 pos: SV_POSITION;
	float2 texCoord: TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	//return float4(1,0,0,1); //the red color

	//float2 texcoord = float2(vI&1, vI>>1);
	//float2 texcoord = float2(pos.x / 2 + 0.5f, -(pos.y) / 2 + 0.5f);

	//return float4((texcoord.x-0.5f)*2, -(texcoord.y-0.5f)*2, 0, 1);

	return tex.Sample(texSam, input.texCoord);
}