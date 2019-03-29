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

struct Camera {
	float4x4 proj_view;
};

struct PerObject {
	float4x4 model;
};

ConstantBuffer<Camera> camera : register(b0);
ConstantBuffer<PerObject> object : register(b1);

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = mul(mul(camera.proj_view, object.model), input.pos);
    output.texCoord = input.texCoord;
    return output;
}