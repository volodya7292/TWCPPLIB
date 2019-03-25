struct VS_INPUT
{
    float4 pos : POSITION;
    float2 texCoord: TEXCOORD;
};

struct VS_OUTPUT
{
    float4 pos: SV_POSITION;
    float2 texCoord: TEXCOORD;
};

struct Foo {
	float4x4 wvpMat;
	//float4x4 govno[2560];
};
ConstantBuffer<Foo> cb : register(b0);

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = mul(cb.wvpMat, input.pos);
    output.texCoord = input.texCoord;
    return output;
}